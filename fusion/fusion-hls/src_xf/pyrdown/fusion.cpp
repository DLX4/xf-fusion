#include "xf_fusion.h"

void pyrdownTop(
		hls::stream<ap_axiu<8,1,1,1>>& _pyr0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrG1
		) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis register both port=_pyr0
#pragma HLS INTERFACE axis register both port=_pyrG1

	int height0 = HEIGHT;
	int width0 = WIDTH;

	int height1 = height0 / 2;
	int width1 = width0 / 2;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyr0(height0, width0);
#pragma HLS STREAM variable=pyr0.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrG1(height1, width1);
#pragma HLS STREAM variable=pyrG1.data depth=1 dim=1

	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyr0, pyr0);

	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyr0, pyrG1);

	xf::xfMat2AXIvideo(pyrG1, _pyrG1);
}

