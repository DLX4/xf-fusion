#include "xf_fusion.h"
#include "fusion_lib.hpp"

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
	fusion::blendOpt<HEIGHT, WIDTH>(pyrLA0, pyrLB0, pyrS0);
	fusion::blendOpt<HEIGHT, WIDTH>(pyrLA1, pyrLB1, pyrS1);
}

void blendTop(
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLA0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLA1,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLB0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLB1,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrS0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrS1
		) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis register both port=_pyrLA0
#pragma HLS INTERFACE axis register both port=_pyrLA1
#pragma HLS INTERFACE axis register both port=_pyrLB0
#pragma HLS INTERFACE axis register both port=_pyrLB1
#pragma HLS INTERFACE axis register both port=_pyrS0
#pragma HLS INTERFACE axis register both port=_pyrS1

	int height0 = HEIGHT;
	int width0 = WIDTH;

	int height1 = height0 / 2;
	int width1 = width0 / 2;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrLA0(height0, width0);
#pragma HLS STREAM variable=pyrLA0.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrLA1(height1, width1);
#pragma HLS STREAM variable=pyrLA1.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrLB0(height0, width0);
#pragma HLS STREAM variable=pyrLB0.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrLB1(height1, width1);
#pragma HLS STREAM variable=pyrLB1.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0);
#pragma HLS STREAM variable=pyrS0.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(height1, width1);
#pragma HLS STREAM variable=pyrS1.data depth=1 dim=1

	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrLA0, pyrLA0);
	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrLA1, pyrLA1);
	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrLB0, pyrLB0);
	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrLB1, pyrLB1);

	blendL(pyrLA0, pyrLA1, pyrLB0, pyrLB1, pyrS0, pyrS1);

	xf::xfMat2AXIvideo(pyrS0, _pyrS0);
	xf::xfMat2AXIvideo(pyrS1, _pyrS1);
}

