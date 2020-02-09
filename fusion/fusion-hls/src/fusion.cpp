#include "xf_fusion.h"
#include "fusion_lib.hpp"

static unsigned char pyrA0Data [HEIGHT * WIDTH] = {0};
static unsigned char pyrA1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char pyrA2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char pyrA3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char pyrA4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char pyrB0Data [HEIGHT * WIDTH] = {0};
static unsigned char pyrB1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char pyrB2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char pyrB3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char pyrB4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char pyrS0Data [HEIGHT * WIDTH] = {0};
static unsigned char pyrS1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char pyrS2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char pyrS3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char pyrS4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char tempA0Data [HEIGHT * WIDTH] = {0};
static unsigned char tempA1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char tempA2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char tempA3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char tempA4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char tempB0Data [HEIGHT * WIDTH] = {0};
static unsigned char tempB1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char tempB2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char tempB3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char tempB4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char tempS0Data [HEIGHT * WIDTH] = {0};
static unsigned char tempS1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char tempS2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char tempS3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char tempS4Data [HEIGHT/16 * WIDTH/16] = {0};
void blend(
		xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcA,
		xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcB,
		xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& dst
		) {
	int height0 = srcA.rows;
	int width0 = srcA.cols;
	int height1 = height0 / 2;
	int width1 = width0 / 2;
	int height2 = height1 / 2;
	int width2 = width1 / 2;
	int height3 = height2 / 2;
	int width3 = width2 / 2;
	int height4 = height3 / 2;
	int width4 = width3 / 2;

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrA0(height0, width0, (void *)pyrA0Data);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrA1(height1, width1, (void *)pyrA1Data);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrA2(height2, width2, (void *)pyrA2Data);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrA3(height3, width3, (void *)pyrA3Data);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrA4(height4, width4, (void *)pyrA4Data);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrA0Temp(height0, width0, (void *)tempA0Data);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrA1Temp(height1, width1, (void *)tempA1Data);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrA2Temp(height2, width2, (void *)tempA2Data);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrA3Temp(height3, width3, (void *)tempA3Data);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrA4Temp(height4, width4, (void *)tempA4Data);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrB0(height0, width0, (void *)pyrB0Data);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrB1(height1, width1, (void *)pyrB1Data);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrB2(height2, width2, (void *)pyrB2Data);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrB3(height3, width3, (void *)pyrB3Data);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrB4(height4, width4, (void *)pyrB4Data);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrB0Temp(height0, width0, (void *)tempB0Data);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrB1Temp(height1, width1, (void *)tempB1Data);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrB2Temp(height2, width2, (void *)tempB2Data);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrB3Temp(height3, width3, (void *)tempB3Data);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrB4Temp(height4, width4, (void *)tempB4Data);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrS0(height0, width0, (void *)pyrS0Data);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrS1(height1, width1, (void *)pyrS1Data);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrS2(height2, width2, (void *)pyrS2Data);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrS3(height3, width3, (void *)pyrS3Data);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrS4(height4, width4, (void *)pyrS4Data);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrS0Temp(height0, width0, (void *)tempS0Data);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrS1Temp(height1, width1, (void *)tempS1Data);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrS2Temp(height2, width2, (void *)tempS2Data);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrS3Temp(height3, width3, (void *)tempS3Data);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrS4Temp(height4, width4, (void *)tempS4Data);

	// 图像A 拉普拉斯金字塔
	fusion::buildLaplacianPyramids<HEIGHT, WIDTH>(
			srcA,
			pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
			pyrA0Temp, pyrA1Temp, pyrA2Temp, pyrA3Temp, pyrA4Temp
			);

	// 图像B 拉普拉斯金字塔
	fusion::buildLaplacianPyramids<HEIGHT, WIDTH>(
			srcB,
			pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
			pyrB0Temp, pyrB1Temp, pyrB2Temp, pyrB3Temp, pyrB4Temp
			);

	// 融合
	fusion::blendLaplacianPyramids<HEIGHT, WIDTH>(pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
						   pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
						   pyrS0, pyrS1, pyrS2, pyrS3, pyrS4,
						   pyrS0Temp, pyrS1Temp, pyrS2Temp, pyrS3Temp, pyrS4Temp,
						   dst);
}
