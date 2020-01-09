/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "GeometryEngine.h"
#include <iostream>
#include <iomanip>
#include <unistd.h>

using namespace std;

#include <QVector2D>
#include <QVector3D>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

#define        MAP_SIZE      256        /* Size Of Our .RAW Height Map */
#define        STEP_SIZE       4        /* Width And Height Of Each Quad */
#define        HEIGHT_OFFSET -0.5f //100, 0.5f
#define        HEIGHT_RATIO  1.0f        /* Ratio That The Y Is Scaled According To The X And Z */
#define        FRAME_CNT_MAX 16

static float g_HeightMap[FRAME_CNT_MAX * MAP_SIZE*MAP_SIZE]; /* Holds The Height Map Data read from the file */

#define NUM_STRIPS  ((MAP_SIZE/STEP_SIZE) - 1)
#define NUM_DEGENS (2 * (NUM_STRIPS - 1))
#define VERTICES_PER_STRIP (2 * (MAP_SIZE/STEP_SIZE))

static int indexBufferCount  = (VERTICES_PER_STRIP * NUM_STRIPS) + NUM_DEGENS;
static int vertexBufferCount = (MAP_SIZE/STEP_SIZE) * (MAP_SIZE/STEP_SIZE);
/* ======================================================================== */
/* Calculate vertices for Color STRIP                                       */
/* ======================================================================== */
static void calcVertexColorSTRIP (VertexData *vertices, int fcnt)
{
    float vX, vY, vZ;
    float tX, tY;
    for (int Y = 0; Y < MAP_SIZE; Y += STEP_SIZE )
    {
        for (int X = 0; X < MAP_SIZE; X += STEP_SIZE )
        {
            /* Get The (X, Y, Z) Value For The Bottom Left Vertex */
            vX = (float)(2*X) / MAP_SIZE - 1.0f;
            vY = (float)(2*Y) / MAP_SIZE - 1.0f;
            vZ = HEIGHT_OFFSET + HEIGHT_RATIO * g_HeightMap[fcnt * MAP_SIZE * MAP_SIZE + Y * MAP_SIZE + X];
            tX = (float)X/(float)MAP_SIZE;
            tY = (float)Y/(float)MAP_SIZE;
            vertices->position = QVector3D(vX, vY, vZ);
            vertices->texCoord = QVector2D(tX, tY);
            vertices ++;
        }
    }
}

/* ======================================================================== */
/* Generate height map for Triangle STRIP                                   */
/* ======================================================================== */
void generateHeightMapTRIANGLE_STRIP_indices(GLushort heightMapIndexData[])
{
    // Now build the index data
    int yLength = (MAP_SIZE/STEP_SIZE);
    int xLength = (MAP_SIZE/STEP_SIZE);
    int offset  = 0;
    for (int y = 0; y < (yLength - 1); y++)
    {
        if (y > 0) {
            // Degenerate begin: repeat first vertex
            heightMapIndexData[offset++] = (unsigned short) (y * yLength);
        }

        for (int x = 0; x < xLength; x++) {
            // One part of the strip
            heightMapIndexData[offset++] = (unsigned short) ((y * yLength) + x);
            heightMapIndexData[offset++] = (unsigned short) (((y + 1) * yLength) + x);
        }

        if (y < (yLength - 2))
        {
            // Degenerate end: repeat last vertex
            heightMapIndexData[offset++] = (unsigned short) (((y + 1) * yLength) + (xLength - 1));
        }
    }
}

/*-------------------------------------------------------------------------------------------------*/
//! [0]
GeometryEngine::GeometryEngine()
    : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    rdIdx = rdCnt = 0;
    wrIdx = wrCnt = 0;
    initializeOpenGLFunctions();
    // Generate 2 VBOs
    arrayBufL.create();
    arrayBufR.create();
    indexBuf.create();
    // Initializes cube geometry and transfers it to VBOs
    initCubeGeometry();
}

bool GeometryEngine::checkLevel (float threshold)
{
  if((wrCnt - rdCnt) >= (int)(FRAME_CNT_MAX * threshold))
  {
    return true;
  } else return false;
}

void GeometryEngine::restart()
{
  rdIdx = wrIdx = 0;
  rdCnt = wrCnt = 0;
}
GeometryEngine::~GeometryEngine()
{
    arrayBufL.destroy();
    arrayBufR.destroy();
    indexBuf.destroy();
}
//! [0]

/* ======================================================================== */
/* Initialize the geometry                                                 */
/* ======================================================================== */
void GeometryEngine::initCubeGeometry()
{
    GLushort *indices    = new GLushort[indexBufferCount];
    generateHeightMapTRIANGLE_STRIP_indices(indices);

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, indexBufferCount * sizeof(GLushort));
//! [1]
}

/* ======================================================================== */
/* Store RGB buffer                                                         */
/* ======================================================================== */
void GeometryEngine::storeHmapBuffer(float *hmap_buffer, int length)
{
    for (int i = 0; i < length; i ++) g_HeightMap[wrIdx * MAP_SIZE * MAP_SIZE + i] = hmap_buffer[i];
    wrIdx ++;
    wrIdx %= FRAME_CNT_MAX;
    wrCnt ++;
}

//! [2]
/* ======================================================================== */
/* Draw the Geometry                                                        */ 
/* ======================================================================== */
void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program)
{
    VertexData *vertices = new VertexData [vertexBufferCount];
    // Tell OpenGL which VBOs to use
    if(rdCnt % 2) arrayBufR.bind();
    else arrayBufL.bind();

    calcVertexColorSTRIP(vertices, rdIdx);
    if(rdCnt % 2) arrayBufL.allocate(vertices, vertexBufferCount * sizeof(VertexData)); // transfer new vertices
    else arrayBufR.allocate(vertices, vertexBufferCount * sizeof(VertexData)); // transfer new vertices

    if(wrCnt > rdCnt)
    {
      rdIdx ++;
      rdIdx %= FRAME_CNT_MAX;
      rdCnt ++;
    }
    // Offset for position
    quintptr offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, indexBufferCount, GL_UNSIGNED_SHORT, 0);
    delete vertices;
}
//! [2]
