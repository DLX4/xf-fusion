#include <string>
#include <math.h>
#include <iostream>
#include <fstream>

#include "xf_fusion.h"

// imwrite
#include "common/xf_sw_utils.h"
// Í·Ïñ
#define AVATAR1_PATH "avatar.jpg"
#define AVATAR2_PATH "avatar2.jpg"

int main (int argc, char** argv)
{
	cv::Mat cvARGB = cv::imread(AVATAR1_PATH);
	cv::Mat cvBRGB = cv::imread(AVATAR2_PATH);

	cv::Mat cvA;
	cv::Mat cvB;
	cv::Mat output;
	cv::cvtColor(cvARGB, cvA, CV_RGB2GRAY);
	cv::cvtColor(cvBRGB, cvB, CV_RGB2GRAY);
	output.create(cvA.rows, cvB.cols, CV_8UC1);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> srcA(cvA.rows, cvA.cols);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> srcB(cvB.rows, cvB.cols);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> dst(cvA.rows, cvB.cols);

	srcA.copyTo(cvA.data);
	srcB.copyTo(cvB.data);

	hls::stream<ap_axiu<8,1,1,1>> _srcA, _srcB, _dst;
	cvMat2AXIvideoxf<XF_NPPC1>(cvA, _srcA);
	cvMat2AXIvideoxf<XF_NPPC1>(cvB, _srcB);
	addTop(HEIGHT, WIDTH, _srcA, _srcB, _dst);
	AXIvideo2cvMatxf<_NPC1, 8>(_dst, output);

	cv::imwrite("blend.jpg", output);
}
