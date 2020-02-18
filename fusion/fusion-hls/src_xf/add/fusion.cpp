#include "xf_fusion.h"

void addTop(
		int _h,
		int _w,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrS1Up,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrS0,
		hls::stream<ap_axiu<8,1,1,1>>& _dst
		) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE s_axilite port=_w
#pragma HLS INTERFACE ap_stable port=_w
#pragma HLS INTERFACE s_axilite port=_h
#pragma HLS INTERFACE ap_stable port=_h
#pragma HLS INTERFACE axis register both port=_dst
#pragma HLS INTERFACE axis register both port=_pyrS1Up
#pragma HLS INTERFACE axis register both port=_pyrS0
#pragma HLS INTERFACE s_axilite register port=return

	int height0 = _h;
	int width0 = _w;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1Up(height0, width0);
#pragma HLS STREAM variable=pyrS1Up.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0);
#pragma HLS STREAM variable=pyrS0.data depth=1 dim=1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> dst(height0, width0);
#pragma HLS STREAM variable=dst.data depth=1 dim=1


	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrS1Up, pyrS1Up);
	xf::AXIvideo2xfMat<8, _TYPE, HEIGHT, WIDTH, _NPC1>(_pyrS0, pyrS0);

	xf::add<XF_CONVERT_POLICY_SATURATE, _TYPE, HEIGHT, WIDTH, _NPC1>(pyrS0, pyrS1Up, dst);

	xf::xfMat2AXIvideo(dst, _dst);
}

