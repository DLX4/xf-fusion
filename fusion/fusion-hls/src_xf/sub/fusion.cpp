#include "xf_fusion.h"

void subTop(
		hls::stream<ap_axiu<8,1,1,1>>& _pyrG1Up,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrG0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrL0
		) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis register both port=_pyrG1Up
#pragma HLS INTERFACE axis register both port=_pyrG0
#pragma HLS INTERFACE axis register both port=_pyrL0

	int height0 = HEIGHT;
	int width0 = WIDTH;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrG1Up(height0, width0);
#pragma HLS STREAM variable=pyrG1Up.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrG0(height0, width0);
#pragma HLS STREAM variable=pyrG0.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrL0(height0, width0);
#pragma HLS STREAM variable=pyrL0.data depth=1 dim=1


	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrG1Up, pyrG1Up);
	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrG0, pyrG0);

	xf::absdiff<_TYPE, HEIGHT, WIDTH, _NPC1>(pyrG1Up, pyrG0, pyrL0);

	xf::xfMat2AXIvideo(pyrL0, _pyrL0);
}

