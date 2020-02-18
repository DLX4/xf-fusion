#include "xf_fusion.h"

void pyrdownTop(
		int _h,
		int _w,
		hls::stream<ap_axiu<8,1,1,1>>& _pyr0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrG1
		) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE s_axilite port=_w
#pragma HLS INTERFACE ap_stable port=_w
#pragma HLS INTERFACE s_axilite port=_h
#pragma HLS INTERFACE ap_stable port=_h
#pragma HLS INTERFACE axis register both port=_pyr0
#pragma HLS INTERFACE axis register both port=_pyrG1
#pragma HLS INTERFACE s_axilite register port=return

	int height0 = _h;
	int width0 = _w;

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

