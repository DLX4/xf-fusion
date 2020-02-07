#include "top.h"
void hls_sobel(AXI_STREAM& INPUT_STREAM, AXI_STREAM&
OUTPUT_STREAM, int rows, int cols)
{
	//Create AXI streaming interfaces for the core
#pragma HLS INTERFACE axis port=INPUT_STREAM
#pragma HLS INTERFACE axis port=OUTPUT_STREAM
#pragma HLS RESOURCE core=AXI_SLAVE variable=rows metadata="-bus_bundle CONTROL_BUS"
#pragma HLS RESOURCE core=AXI_SLAVE variable=cols metadata="-bus_bundle CONTROL_BUS"
#pragma HLS RESOURCE core=AXI_SLAVE variable=return metadata="-bus_bundle CONTROL_BUS"
#pragma HLS INTERFACE ap_stable port=rows
#pragma HLS INTERFACE ap_stable port=cols
	RGB_IMAGE img_0(rows, cols);
	RGB_IMAGE img_1(rows, cols);
	RGB_IMAGE img_2(rows, cols);
	RGB_IMAGE img_3(rows, cols);
	RGB_IMAGE img_4(rows, cols);
	RGB_IMAGE img_5(rows, cols);
	RGB_PIXEL pix(50, 50, 50);
#pragma HLS dataflow
	hls::AXIvideo2Mat(INPUT_STREAM, img_0);
	hls::Sobel<1,0,3>(img_0, img_1);
	hls::SubS(img_1, pix, img_2);
	hls::Scale(img_2, img_3, 2, 0);
	hls::Erode(img_3, img_4);
	hls::Dilate(img_4, img_5);
	hls::Mat2AXIvideo(img_5, OUTPUT_STREAM);
}
