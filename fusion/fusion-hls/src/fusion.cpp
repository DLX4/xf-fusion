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
#pragma HLS INLINE OFF

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

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0(1, 1, (void *)pyrA0Data);pyrA0.allocatedFlag = 3; pyrA0.rows = height0; pyrA0.cols = width0; pyrA0.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1(1, 1, (void *)pyrA1Data);pyrA1.allocatedFlag = 3; pyrA1.rows = height1; pyrA1.cols = width1; pyrA1.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2(1, 1, (void *)pyrA2Data);pyrA2.allocatedFlag = 3; pyrA2.rows = height2; pyrA2.cols = width2; pyrA2.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA3(1, 1, (void *)pyrA3Data);pyrA3.allocatedFlag = 3; pyrA3.rows = height3; pyrA3.cols = width3; pyrA3.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA4(1, 1, (void *)pyrA4Data);pyrA4.allocatedFlag = 3; pyrA4.rows = height4; pyrA4.cols = width4; pyrA4.size = height4 * width4;
	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0TempScale1(1, 1, (void *)tempA0Data); pyrA0TempScale1.allocatedFlag = 3; pyrA0TempScale1.rows = height0; pyrA0TempScale1.cols = width0; pyrA0TempScale1.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1TempScale1(1, 1, (void *)tempA1Data); pyrA1TempScale1.allocatedFlag = 3; pyrA1TempScale1.rows = height1; pyrA1TempScale1.cols = width1; pyrA1TempScale1.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2TempScale1(1, 1, (void *)tempA2Data); pyrA2TempScale1.allocatedFlag = 3; pyrA2TempScale1.rows = height2; pyrA2TempScale1.cols = width2; pyrA2TempScale1.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA3TempScale1(1, 1, (void *)tempA3Data); pyrA3TempScale1.allocatedFlag = 3; pyrA3TempScale1.rows = height3; pyrA3TempScale1.cols = width3; pyrA3TempScale1.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA4TempScale1(1, 1, (void *)tempA4Data); pyrA4TempScale1.allocatedFlag = 3; pyrA4TempScale1.rows = height4; pyrA4TempScale1.cols = width4; pyrA4TempScale1.size = height4 * width4;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA0TempScale2(1, 1, (void *)tempA0Data); pyrA0TempScale2.allocatedFlag = 3; pyrA0TempScale2.rows = height0; pyrA0TempScale2.cols = width0; pyrA0TempScale2.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA1TempScale2(1, 1, (void *)tempA1Data); pyrA1TempScale2.allocatedFlag = 3; pyrA1TempScale2.rows = height1; pyrA1TempScale2.cols = width1; pyrA1TempScale2.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA2TempScale2(1, 1, (void *)tempA2Data); pyrA2TempScale2.allocatedFlag = 3; pyrA2TempScale2.rows = height2; pyrA2TempScale2.cols = width2; pyrA2TempScale2.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA3TempScale2(1, 1, (void *)tempA3Data); pyrA3TempScale2.allocatedFlag = 3; pyrA3TempScale2.rows = height3; pyrA3TempScale2.cols = width3; pyrA3TempScale2.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA4TempScale2(1, 1, (void *)tempA4Data); pyrA4TempScale2.allocatedFlag = 3; pyrA4TempScale2.rows = height4; pyrA4TempScale2.cols = width4; pyrA4TempScale2.size = height4 * width4;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0(1, 1, (void *)pyrB0Data); pyrB0.allocatedFlag = 3; pyrB0.rows = height0; pyrB0.cols = width0; pyrB0.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1(1, 1, (void *)pyrB1Data); pyrB1.allocatedFlag = 3; pyrB1.rows = height1; pyrB1.cols = width1; pyrB1.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2(1, 1, (void *)pyrB2Data); pyrB2.allocatedFlag = 3; pyrB2.rows = height2; pyrB2.cols = width2; pyrB2.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB3(1, 1, (void *)pyrB3Data); pyrB3.allocatedFlag = 3; pyrB3.rows = height3; pyrB3.cols = width3; pyrB3.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB4(1, 1, (void *)pyrB4Data); pyrB4.allocatedFlag = 3; pyrB4.rows = height4; pyrB4.cols = width4; pyrB4.size = height4 * width4;

	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0TempScale1(1, 1, (void *)tempB0Data); pyrB0TempScale1.allocatedFlag = 3; pyrB0TempScale1.rows = height0; pyrB0TempScale1.cols = width0; pyrB0TempScale1.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1TempScale1(1, 1, (void *)tempB1Data); pyrB1TempScale1.allocatedFlag = 3; pyrB1TempScale1.rows = height1; pyrB1TempScale1.cols = width1; pyrB1TempScale1.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2TempScale1(1, 1, (void *)tempB2Data); pyrB2TempScale1.allocatedFlag = 3; pyrB2TempScale1.rows = height2; pyrB2TempScale1.cols = width2; pyrB2TempScale1.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB3TempScale1(1, 1, (void *)tempB3Data); pyrB3TempScale1.allocatedFlag = 3; pyrB3TempScale1.rows = height3; pyrB3TempScale1.cols = width3; pyrB3TempScale1.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB4TempScale1(1, 1, (void *)tempB4Data); pyrB4TempScale1.allocatedFlag = 3; pyrB4TempScale1.rows = height4; pyrB4TempScale1.cols = width4; pyrB4TempScale1.size = height4 * width4;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB0TempScale2(1, 1, (void *)tempB0Data); pyrB0TempScale2.allocatedFlag = 3; pyrB0TempScale2.rows = height0; pyrB0TempScale2.cols = width0; pyrB0TempScale2.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB1TempScale2(1, 1, (void *)tempB1Data); pyrB1TempScale2.allocatedFlag = 3; pyrB1TempScale2.rows = height1; pyrB1TempScale2.cols = width1; pyrB1TempScale2.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB2TempScale2(1, 1, (void *)tempB2Data); pyrB2TempScale2.allocatedFlag = 3; pyrB2TempScale2.rows = height2; pyrB2TempScale2.cols = width2; pyrB2TempScale2.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB3TempScale2(1, 1, (void *)tempB3Data); pyrB3TempScale2.allocatedFlag = 3; pyrB3TempScale2.rows = height3; pyrB3TempScale2.cols = width3; pyrB3TempScale2.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB4TempScale2(1, 1, (void *)tempB4Data); pyrB4TempScale2.allocatedFlag = 3; pyrB4TempScale2.rows = height4; pyrB4TempScale2.cols = width4; pyrB4TempScale2.size = height4 * width4;


	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(1, 1, (void *)pyrS0Data); pyrS0.allocatedFlag = 3; pyrS0.rows = height0; pyrS0.cols = width0; pyrS0.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(1, 1, (void *)pyrS1Data); pyrS1.allocatedFlag = 3; pyrS1.rows = height1; pyrS1.cols = width1; pyrS1.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2(1, 1, (void *)pyrS2Data); pyrS2.allocatedFlag = 3; pyrS2.rows = height2; pyrS2.cols = width2; pyrS2.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS3(1, 1, (void *)pyrS3Data); pyrS3.allocatedFlag = 3; pyrS3.rows = height3; pyrS3.cols = width3; pyrS3.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS4(1, 1, (void *)pyrS4Data); pyrS4.allocatedFlag = 3; pyrS4.rows = height4; pyrS4.cols = width4; pyrS4.size = height4 * width4;

	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0TempScale1(1, 1, (void *)tempS0Data); pyrS0TempScale1.allocatedFlag = 3; pyrS0TempScale1.rows = height0; pyrS0TempScale1.cols = width0; pyrS0TempScale1.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1TempScale1(1, 1, (void *)tempS1Data); pyrS1TempScale1.allocatedFlag = 3; pyrS1TempScale1.rows = height1; pyrS1TempScale1.cols = width1; pyrS1TempScale1.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2TempScale1(1, 1, (void *)tempS2Data); pyrS2TempScale1.allocatedFlag = 3; pyrS2TempScale1.rows = height2; pyrS2TempScale1.cols = width2; pyrS2TempScale1.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS3TempScale1(1, 1, (void *)tempS3Data); pyrS3TempScale1.allocatedFlag = 3; pyrS3TempScale1.rows = height3; pyrS3TempScale1.cols = width3; pyrS3TempScale1.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS4TempScale1(1, 1, (void *)tempS4Data); pyrS4TempScale1.allocatedFlag = 3; pyrS4TempScale1.rows = height4; pyrS4TempScale1.cols = width4; pyrS4TempScale1.size = height4 * width4;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS0TempScale2(1, 1, (void *)tempS0Data); pyrS0TempScale2.allocatedFlag = 3; pyrS0TempScale2.rows = height0; pyrS0TempScale2.cols = width0; pyrS0TempScale2.size = height0 * width0;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS1TempScale2(1, 1, (void *)tempS1Data); pyrS1TempScale2.allocatedFlag = 3; pyrS1TempScale2.rows = height1; pyrS1TempScale2.cols = width1; pyrS1TempScale2.size = height1 * width1;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS2TempScale2(1, 1, (void *)tempS2Data); pyrS2TempScale2.allocatedFlag = 3; pyrS2TempScale2.rows = height2; pyrS2TempScale2.cols = width2; pyrS2TempScale2.size = height2 * width2;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS3TempScale2(1, 1, (void *)tempS3Data); pyrS3TempScale2.allocatedFlag = 3; pyrS3TempScale2.rows = height3; pyrS3TempScale2.cols = width3; pyrS3TempScale2.size = height3 * width3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS4TempScale2(1, 1, (void *)tempS4Data); pyrS4TempScale2.allocatedFlag = 3; pyrS4TempScale2.rows = height4; pyrS4TempScale2.cols = width4; pyrS4TempScale2.size = height4 * width4;

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
