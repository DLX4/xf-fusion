#include "xf_fusion.h"
#include "fusion_lib.hpp"

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

	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> tempScale1(height0, width0);

	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> tempScale2(height0*2, width0*2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1(height1, width1);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2(height2, width2);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA3(height3, width3);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA4(height4, width4);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1(height1, width1);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2(height2, width2);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB3(height3, width3);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB4(height4, width4);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(height1, width1);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2(height2, width2);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS3(height3, width3);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS4(height4, width4);

	tempScale1.allocatedFlag = 3; tempScale1.rows = height0; tempScale1.cols = width0; tempScale1.size = height0 * width0;
	tempScale2.allocatedFlag = 3; tempScale2.rows = height0*2; tempScale2.cols = width0*2; tempScale2.size = height0 * width0 * 4;

	pyrA0.allocatedFlag = 3; pyrA0.rows = height0; pyrA0.cols = width0; pyrA0.size = height0 * width0;
    pyrA1.allocatedFlag = 3; pyrA1.rows = height1; pyrA1.cols = width1; pyrA1.size = height1 * width1;
    pyrA2.allocatedFlag = 3; pyrA2.rows = height2; pyrA2.cols = width2; pyrA2.size = height2 * width2;
    pyrA3.allocatedFlag = 3; pyrA3.rows = height3; pyrA3.cols = width3; pyrA3.size = height3 * width3;
    pyrA4.allocatedFlag = 3; pyrA4.rows = height4; pyrA4.cols = width4; pyrA4.size = height4 * width4;

	pyrB0.allocatedFlag = 3; pyrB0.rows = height0; pyrB0.cols = width0; pyrB0.size = height0 * width0;
    pyrB1.allocatedFlag = 3; pyrB1.rows = height1; pyrB1.cols = width1; pyrB1.size = height1 * width1;
    pyrB2.allocatedFlag = 3; pyrB2.rows = height2; pyrB2.cols = width2; pyrB2.size = height2 * width2;
    pyrB3.allocatedFlag = 3; pyrB3.rows = height3; pyrB3.cols = width3; pyrB3.size = height3 * width3;
    pyrB4.allocatedFlag = 3; pyrB4.rows = height4; pyrB4.cols = width4; pyrB4.size = height4 * width4;

    pyrS0.allocatedFlag = 3; pyrS0.rows = height0; pyrS0.cols = width0; pyrS0.size = height0 * width0;
    pyrS1.allocatedFlag = 3; pyrS1.rows = height1; pyrS1.cols = width1; pyrS1.size = height1 * width1;
    pyrS2.allocatedFlag = 3; pyrS2.rows = height2; pyrS2.cols = width2; pyrS2.size = height2 * width2;
    pyrS3.allocatedFlag = 3; pyrS3.rows = height3; pyrS3.cols = width3; pyrS3.size = height3 * width3;
    pyrS4.allocatedFlag = 3; pyrS4.rows = height4; pyrS4.cols = width4; pyrS4.size = height4 * width4;
	// 图像A
	// 往下构造高斯金字塔
	fusion::dstCopyFromSrc<HEIGHT, WIDTH>(srcA, pyrA0);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrA0, pyrA1);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrA1, pyrA2);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrA2, pyrA3);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrA3, pyrA4);

	// 拉普拉斯金字塔
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrA1, pyrA0, tempScale1, tempScale2, pyrA0);
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrA2, pyrA1, tempScale1, tempScale2, pyrA1);
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrA3, pyrA2, tempScale1, tempScale2, pyrA2);
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrA4, pyrA3, tempScale1, tempScale2, pyrA3);

	// 图像B 拉普拉斯金字塔
	fusion::dstCopyFromSrc<HEIGHT, WIDTH>(srcB, pyrB0);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrB0, pyrB1);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrB1, pyrB2);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrB2, pyrB3);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrB3, pyrB4);

	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrB1, pyrB0, tempScale1, tempScale2, pyrB0);
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrB2, pyrB1, tempScale1, tempScale2, pyrB1);
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrB3, pyrB2, tempScale1, tempScale2, pyrB2);
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrB4, pyrB3, tempScale1, tempScale2, pyrB3);

	// 拉普拉斯金字塔各层分别融合 0 1 2 3 4
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrA0, pyrB0, pyrS0);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrA0, pyrB0, pyrS0);
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrA1, pyrB1, pyrS1);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrA1, pyrB1, pyrS1);
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrA2, pyrB2, pyrS2);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrA2, pyrB2, pyrS2);
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrA3, pyrB3, pyrS3);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrA3, pyrB3, pyrS3);
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrA4, pyrB4, pyrS4);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrA4, pyrB4, pyrS4);

	// 重建 从小到大
	fusion::lapPyrUpAddLevel<HEIGHT, WIDTH>(pyrS4, pyrS3, tempScale1, tempScale2, pyrS3);
	fusion::lapPyrUpAddLevel<HEIGHT, WIDTH>(pyrS3, pyrS2, tempScale1, tempScale2, pyrS2);
	fusion::lapPyrUpAddLevel<HEIGHT, WIDTH>(pyrS2, pyrS1, tempScale1, tempScale2, pyrS1);
	fusion::lapPyrUpAddLevel<HEIGHT, WIDTH>(pyrS1, pyrS0, tempScale1, tempScale2, pyrS0);

	// 调整亮度
	double min = 0.0;
	double max = 0.0;
	fusion::meanStdDevWrapper<HEIGHT, WIDTH>(pyrS0, min, max);
	fusion::restoreBrightness<HEIGHT, WIDTH>(pyrS0, dst, min, max);
}
