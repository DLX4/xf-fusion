#include "xf_fusion.h"

void pyrupTop(
		int _h,
		int _w,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrX,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrUp
		) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE s_axilite port=_w
#pragma HLS INTERFACE ap_stable port=_w
#pragma HLS INTERFACE s_axilite port=_h
#pragma HLS INTERFACE ap_stable port=_h
#pragma HLS INTERFACE axis register both port=_pyrX
#pragma HLS INTERFACE axis register both port=_pyrUp
#pragma HLS INTERFACE s_axilite register port=return

	int height0 = _h;
	int width0 = _w;

	int height1 = height0 / 2;
	int width1 = width0 / 2;

	xf::Mat<_TYPE, HEIGHT/2, WIDTH/2, _NPC1> pyrX(height1, width1);
#pragma HLS STREAM variable=pyrX.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrUp(height0, width0);
#pragma HLS STREAM variable=pyrUp.data depth=1 dim=1

	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT/2, WIDTH/2, _NPC1>(_pyrX, pyrX);

	xf::pyrUp<_TYPE, HEIGHT/2, WIDTH/2,  _NPC1>(pyrX, pyrUp);

	xf::xfMat2AXIvideo(pyrUp, _pyrUp);
}

