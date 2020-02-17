#include "xf_fusion.h"
#include "fusion_lib.hpp"

// 构造高斯金字塔
void buildG(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyr0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyr1
		)
{
#pragma HLS dataflow
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyr0, pyr1);
	// xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyr1, pyr2);
}

// 图像A拉普拉斯金字塔  因为xfopencv接口的原因不能dataflow
void buildL(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrG0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrG1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrL0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrL1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& tempScale1L1,
		xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1>& tempScale2L1
		)
{
#pragma HLS dataflow
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrG1, pyrG0, tempScale1L1, tempScale2L1, pyrL0);
	// fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrG2, pyrG1, tempScale1L2, tempScale2L2, pyrL1);
}

void blendLBorder(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1
) {
#pragma HLS dataflow
	// 分别融合图像AB各层拉普拉斯金字塔
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrLA0, pyrLB0, pyrS0);
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrLA1, pyrLB1, pyrS1);
}

void blendL(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1
) {
#pragma HLS dataflow
	// 分别融合图像AB各层拉普拉斯金字塔
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrLA0, pyrLB0, pyrS0);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrLA1, pyrLB1, pyrS1);
}
void blend(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& dst,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& tempScale1A,
		xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1>& tempScale2A,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& tempScale1B,
		xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1>& tempScale2B
		) {
// #pragma HLS INLINE OFF
	// 构建高斯金字塔
	buildG(pyrA0, pyrA1);
	buildG(pyrB0, pyrB1);

	// 构建拉普拉斯金字塔
	buildL(pyrA0, pyrA1, pyrA0, pyrA1, tempScale1A, tempScale2A);
	buildL(pyrB0, pyrB1, pyrB0, pyrB1, tempScale1B, tempScale2B);

	blendL(pyrA0, pyrA1, pyrB0, pyrB1, pyrS0, pyrS1);
	blendLBorder(pyrA0, pyrA1, pyrB0, pyrB1, pyrS0, pyrS1);

	// 累加拉普拉斯金字塔
	fusion::lapPyrUpAddLevel<HEIGHT, WIDTH>(pyrS1, pyrS0, tempScale1A, tempScale2A, pyrS0);

	// 调整亮度
	double min = 0.0;
	double max = 0.0;
	fusion::meanStdDevWrapper<HEIGHT, WIDTH>(pyrS0, min, max);
	fusion::restoreBrightness<HEIGHT, WIDTH>(pyrS0, dst, min, max);
}

void blendTop(
		int _h,
		int _w,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrA0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrB0,
		hls::stream<ap_axiu<8,1,1,1>>& _dst
		) {

	int height0 = _h;
	int width0 = _w;
	int height1 = height0 / 2;
	int width1 = width0 / 2;
	int height2 = height1 / 2;
	int width2 = width1 / 2;
	int height3 = height2 / 2;
	int width3 = width2 / 2;
	int height4 = height3 / 2;
	int width4 = width3 / 2;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> tempScale1A(height0, width0);
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> tempScale2A(height0, width0);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> tempScale1B(height0, width0);
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> tempScale2B(height0, width0);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> dst(height0, width0);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1(height1, width1);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1(height1, width1);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(height1, width1);

	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrA0, pyrA0);
	//xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrA1, pyrA1);
	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrB0, pyrB0);
	//xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrB1, pyrB1);
	//xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrS0, pyrS0);
	//xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrS1, pyrS1);

	blend(pyrA0, pyrA1, pyrB0, pyrB1, pyrS0, pyrS1, dst, tempScale1A, tempScale2A, tempScale1B, tempScale2B);

	xf::xfMat2AXIvideo(dst, _dst);
}

