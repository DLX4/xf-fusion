#include <string>
#include <math.h>
#include <iostream>
#include <fstream>

#include "xf_fusion.h"

// imwrite
#include "common/xf_sw_utils.h"
// Í·Ïñ
#define SRC_IMG "pyrS0.jpg"

int main (int argc, char** argv)
{
	cv::Mat src = cv::imread(SRC_IMG, 0);

	cv::Mat dst;
	dst.create(src.rows/2, src.cols/2, CV_8UC1);

	hls::stream<ap_axiu<8,1,1,1>> _src, _dst;
	cvMat2AXIvideoxf<XF_NPPC1>(src, _src);

	pyrdownTop(_src, _dst);
	AXIvideo2cvMatxf<_NPC1, 8>(_dst, dst);

	cv::imwrite("dst.jpg", dst);
}
