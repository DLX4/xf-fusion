#include <string>
#include <math.h>
#include <iostream>
#include <fstream>

#include "xf_fusion.h"

// imwrite
#include "common/xf_sw_utils.h"
// Í·Ïñ
#define PYRA0_PATH "pyrA0.jpg"
#define PYRB0_PATH "pyrB0.jpg"
#define PYRA1_PATH "pyrA1.jpg"
#define PYRB1_PATH "pyrB1.jpg"

int main (int argc, char** argv)
{
	cv::Mat pyrA0 = cv::imread(PYRA0_PATH, 0);
	cv::Mat pyrB0 = cv::imread(PYRB0_PATH, 0);

	cv::Mat pyrA1 = cv::imread(PYRA1_PATH, 0);
	cv::Mat pyrB1 = cv::imread(PYRB1_PATH, 0);

	cv::Mat dst0;
	cv::Mat dst1;
	dst0.create(pyrA0.rows, pyrA0.cols, CV_8UC1);
	dst1.create(pyrA0.rows/2, pyrA0.cols/2, CV_8UC1);

	hls::stream<ap_axiu<8,1,1,1>> _pyrA0, _pyrB0, _pyrA1, _pyrB1, _dst0, _dst1;
	cvMat2AXIvideoxf<XF_NPPC1>(pyrA0, _pyrA0);
	cvMat2AXIvideoxf<XF_NPPC1>(pyrB0, _pyrB0);
	cvMat2AXIvideoxf<XF_NPPC1>(pyrA1, _pyrA1);
	cvMat2AXIvideoxf<XF_NPPC1>(pyrB1, _pyrB1);

	blendTop(_pyrA0, _pyrA1, _pyrB0, _pyrB1, _dst0, _dst1);
	AXIvideo2cvMatxf<_NPC1, 8>(_dst0, dst0);
	AXIvideo2cvMatxf<_NPC1, 8>(_dst1, dst1);

	cv::imwrite("_dst0.jpg", dst0);
	cv::imwrite("_dst1.jpg", dst1);
}
