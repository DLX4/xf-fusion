
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include "ap_int.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

#include "xf_fusion.h"
#include "fusion_lib.hpp"
// imwrite
#include "common/xf_sw_utils.h"
// Í·Ïñ
#define AVATAR1_PATH "avatar.jpg"
#define AVATAR2_PATH "avatar2.jpg"

void init(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1
		) {
	int height0 = pyrA0.rows;
	int width0 = pyrA0.cols;
	int height1 = height0 / 2;
	int width1 = width0 / 2;
	int height2 = height1 / 2;
	int width2 = width1 / 2;
	int height3 = height2 / 2;
	int width3 = width2 / 2;
	int height4 = height3 / 2;
	int width4 = width3 / 2;

	pyrA0.allocatedFlag = 3; pyrA0.rows = height0; pyrA0.cols = width0; pyrA0.size = height0 * width0;
	pyrA1.allocatedFlag = 3; pyrA1.rows = height1; pyrA1.cols = width1; pyrA1.size = height1 * width1;

	pyrB0.allocatedFlag = 3; pyrB0.rows = height0; pyrB0.cols = width0; pyrB0.size = height0 * width0;
	pyrB1.allocatedFlag = 3; pyrB1.rows = height1; pyrB1.cols = width1; pyrB1.size = height1 * width1;

	pyrS0.allocatedFlag = 3; pyrS0.rows = height0; pyrS0.cols = width0; pyrS0.size = height0 * width0;
	pyrS1.allocatedFlag = 3; pyrS1.rows = height1; pyrS1.cols = width1; pyrS1.size = height1 * width1;
}


void testBlend() {
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
	// ÈÚºÏ ----------------------------------------------------------
	int height0 = srcA.rows;
	int width0 = srcA.cols;
	int height1 = height0 / 2;
	int width1 = width0 / 2;
	int height2 = height1 / 2;
	int width2 = width1 / 2;
	int height3 = height2 / 2;
	int width3 = width2 / 2;
	int height4 = height3 / 2;
	int width4 = width3 / 2;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> tempScale1A(height0, width0);
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> tempScale2A(height0*2, width0*2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> tempScale1B(height0, width0);
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> tempScale2B(height0*2, width0*2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1(height1, width1);
	// xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2(height2, width2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1(height1, width1);
	// xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2(height2, width2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(height1, width1);
	// xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2(height2, width2);

	fusion::dstCopyFromSrc<HEIGHT, WIDTH>(srcA, pyrA0);
	fusion::dstCopyFromSrc<HEIGHT, WIDTH>(srcB, pyrB0);

	blend(pyrA0, pyrA1, pyrB0, pyrB1, pyrS0, pyrS1, dst, tempScale1A, tempScale2A, tempScale1B, tempScale2B);

	xf::imwrite("blend.jpg", dst);
}

int main (int argc, char** argv)
{
	testBlend();
}
