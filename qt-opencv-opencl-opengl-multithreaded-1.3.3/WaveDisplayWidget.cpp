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

#include "WaveDisplayWidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <math.h>

using namespace std;

WaveDisplayWidget::WaveDisplayWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    texture(0)
{
  QWidget::setFixedSize(600, 400);
  //Delete widget upon close - otherwise it only stays hidden
  setAttribute(Qt::WA_DeleteOnClose);
}

void WaveDisplayWidget::closeEvent(QCloseEvent *event)
{
  *gl_stopped = 2;
  event->accept();
}

void WaveDisplayWidget::setStm(int *stmVar)
{
   gl_stopped = stmVar;
}

WaveDisplayWidget::~WaveDisplayWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    delete geometries;
    doneCurrent();
}

//! [1]
/* ======================================================================== */
/* Timer event  handling                                                    */
/* ======================================================================== */
void WaveDisplayWidget::timerEvent(QTimerEvent *)
{
    update();
    //repaint();
}
//! [1]

/* ======================================================================== */
/* Initialize GL                                                            */
/* ======================================================================== */
void WaveDisplayWidget::initializeGL()
{
    rotation = QQuaternion (0.0f, 0.0f, -1.0f, 0.3f);
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    // Create geometry engine
    geometries = new GeometryEngine;
    // Use QBasicTimer because its faster than QTimer
    timer.start(120.0f, this);
}

//! [3]
/* ======================================================================== */
/* Initialize shaders                                                       */
/* ======================================================================== */
void WaveDisplayWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
    {
        qDebug() << "Vertex shader compile returned error:" << program.log();
        close();
    }
    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
    {
        qDebug() << "Fragment shader compile returned error:" << program.log();
        close();
    }

    // Link shader pipeline
    if (!program.link())
    {
        qDebug() << "Shader link returned error!:" << program.log();
        close();
    }
    // Bind shader pipeline for use
    if (!program.bind()) {
        qDebug() << "Shader bind returned error!:" << program.log();
        close();
    }
}
//! [3]

//! [4]
/* ======================================================================== */
/* Initialize textures                                                      */
/* ======================================================================== */
void WaveDisplayWidget::initTextures()
{
    // Load lake-surface.jpg image
    texture = new QOpenGLTexture(QImage(":/lake-surface.jpg").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}
//! [4]

//! [5]
/* ======================================================================== */
/* handle resizing of GL                                                    */
/* ======================================================================== */
void WaveDisplayWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 2.0, far plane to 8.0, field of view 15 degrees
    const qreal zNear = 2.0, zFar = 8.0, fov = 15.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

/* ======================================================================== */
/* GL paint                                                                 */
/* ======================================================================== */
void WaveDisplayWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);
//! [6]

    // Use texture unit 0 which contains lake-surface.jpg
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(&program);
}
