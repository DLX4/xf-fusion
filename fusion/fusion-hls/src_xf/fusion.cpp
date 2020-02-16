#include "xf_fusion.h"
#include "fusion_lib.hpp"


void blendLBorder(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS2
) {
#pragma HLS dataflow
	// 分别融合图像AB各层拉普拉斯金字塔
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrLA0, pyrLB0, pyrS0);
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrLA1, pyrLB1, pyrS1);
	fusion::blendLaplacianPyramidsBorder<HEIGHT, WIDTH>(pyrLA2, pyrLB2, pyrS2);
}

void blendL(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS2
) {
#pragma HLS dataflow
	// 分别融合图像AB各层拉普拉斯金字塔
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrLA0, pyrLB0, pyrS0);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrLA1, pyrLB1, pyrS1);
	fusion::blendLaplacianPyramidsByRE2<HEIGHT, WIDTH>(pyrLA2, pyrLB2, pyrS2);
}
void blend(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLA2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrLB2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS2
		) {
#pragma HLS INLINE OFF
#pragma HLS dataflow
	blendL(pyrLA0, pyrLA1, pyrLA2, pyrLB0, pyrLB1, pyrLB2, pyrS0, pyrS1, pyrS2);
	// blendLBorder(pyrLA0, pyrLA1, pyrLA2, pyrLB0, pyrLB1, pyrLB2, pyrS0, pyrS1, pyrS2);
}
