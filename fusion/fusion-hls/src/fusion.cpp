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
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& srcA,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& srcB,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& dst
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

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0(height0, width0, (void *)pyrA0Data);pyrA0.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1(height1, width1, (void *)pyrA1Data);pyrA1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2(height2, width2, (void *)pyrA2Data);pyrA2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA3(height3, width3, (void *)pyrA3Data);pyrA3.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA4(height4, width4, (void *)pyrA4Data);pyrA4.allocatedFlag = 3;
	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0TempScale1(height0, width0, (void *)tempA0Data); pyrA0TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1TempScale1(height1, width1, (void *)tempA1Data); pyrA1TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2TempScale1(height2, width2, (void *)tempA2Data); pyrA2TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA3TempScale1(height3, width3, (void *)tempA3Data); pyrA3TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA4TempScale1(height4, width4, (void *)tempA4Data); pyrA4TempScale1.allocatedFlag = 3;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA0TempScale2(height0, width0, (void *)tempA0Data); pyrA0TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA1TempScale2(height1, width1, (void *)tempA1Data); pyrA1TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA2TempScale2(height2, width2, (void *)tempA2Data); pyrA2TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA3TempScale2(height3, width3, (void *)tempA3Data); pyrA3TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA4TempScale2(height4, width4, (void *)tempA4Data); pyrA4TempScale2.allocatedFlag = 3;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0(height0, width0, (void *)pyrB0Data); pyrB0.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1(height1, width1, (void *)pyrB1Data); pyrB1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2(height2, width2, (void *)pyrB2Data); pyrB2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB3(height3, width3, (void *)pyrB3Data); pyrB3.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB4(height4, width4, (void *)pyrB4Data); pyrB4.allocatedFlag = 3;

	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0TempScale1(height0, width0, (void *)tempB0Data); pyrB0TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1TempScale1(height1, width1, (void *)tempB1Data); pyrB1TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2TempScale1(height2, width2, (void *)tempB2Data); pyrB2TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB3TempScale1(height3, width3, (void *)tempB3Data); pyrB3TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB4TempScale1(height4, width4, (void *)tempB4Data); pyrB4TempScale1.allocatedFlag = 3;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB0TempScale2(height0, width0, (void *)tempB0Data); pyrB0TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB1TempScale2(height1, width1, (void *)tempB1Data); pyrB1TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB2TempScale2(height2, width2, (void *)tempB2Data); pyrB2TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB3TempScale2(height3, width3, (void *)tempB3Data); pyrB3TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB4TempScale2(height4, width4, (void *)tempB4Data); pyrB4TempScale2.allocatedFlag = 3;


	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0, (void *)pyrS0Data); pyrS0.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(height1, width1, (void *)pyrS1Data); pyrS1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2(height2, width2, (void *)pyrS2Data); pyrS2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS3(height3, width3, (void *)pyrS3Data); pyrS3.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS4(height4, width4, (void *)pyrS4Data); pyrS4.allocatedFlag = 3;

	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0TempScale1(height0, width0, (void *)tempS0Data); pyrS0TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1TempScale1(height1, width1, (void *)tempS1Data); pyrS1TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2TempScale1(height2, width2, (void *)tempS2Data); pyrS2TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS3TempScale1(height3, width3, (void *)tempS3Data); pyrS3TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS4TempScale1(height4, width4, (void *)tempS4Data); pyrS4TempScale1.allocatedFlag = 3;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS0TempScale2(height0, width0, (void *)tempS0Data); pyrS0TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS1TempScale2(height1, width1, (void *)tempS1Data); pyrS1TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS2TempScale2(height2, width2, (void *)tempS2Data); pyrS2TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS3TempScale2(height3, width3, (void *)tempS3Data); pyrS3TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS4TempScale2(height4, width4, (void *)tempS4Data); pyrS4TempScale2.allocatedFlag = 3;

	// 图像A 拉普拉斯金字塔
	fusion::buildLaplacianPyramids<HEIGHT, WIDTH>(
			srcA,
			pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
			pyrA0TempScale1, pyrA1TempScale1, pyrA2TempScale1, pyrA3TempScale1, pyrA4TempScale1,
			pyrA0TempScale2, pyrA1TempScale2, pyrA2TempScale2, pyrA3TempScale2, pyrA4TempScale2
			);

	// 图像B 拉普拉斯金字塔
	fusion::buildLaplacianPyramids<HEIGHT, WIDTH>(
			srcB,
			pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
			pyrB0TempScale1, pyrB1TempScale1, pyrB2TempScale1, pyrB3TempScale1, pyrB4TempScale1,
			pyrB0TempScale2, pyrB1TempScale2, pyrB2TempScale2, pyrB3TempScale2, pyrB4TempScale2
			);

	// 融合
	fusion::blendLaplacianPyramids<HEIGHT, WIDTH>(pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
						   pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
						   pyrS0, pyrS1, pyrS2, pyrS3, pyrS4,
						   pyrS0TempScale1, pyrS1TempScale1, pyrS2TempScale1, pyrS3TempScale1, pyrS4TempScale1,
						   pyrS0TempScale2, pyrS1TempScale2, pyrS2TempScale2, pyrS3TempScale2, pyrS4TempScale2,
						   dst);
}
