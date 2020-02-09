#include "xf_fusion.h"
#include "fusion_lib.hpp"

void blend(xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcA, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcB, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& dst) {
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

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrA0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrA1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrA2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrA3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrA4(height4, width4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrB0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrB1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrB2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrB3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrB4(height4, width4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrS0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrS1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrS2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrS3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrS4(height4, width4);

	// 图像A 拉普拉斯金字塔
	fusion::buildLaplacianPyramids<HEIGHT, WIDTH>(srcA, pyrA0, pyrA1, pyrA2, pyrA3, pyrA4);

	// 图像B 拉普拉斯金字塔
	fusion::buildLaplacianPyramids<HEIGHT, WIDTH>(srcB, pyrB0, pyrB1, pyrB2, pyrB3, pyrB4);

	// 融合
	fusion::blendLaplacianPyramids<HEIGHT, WIDTH>(pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
						   pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
						   pyrS0, pyrS1, pyrS2, pyrS3, pyrS4,
						   dst);
}
