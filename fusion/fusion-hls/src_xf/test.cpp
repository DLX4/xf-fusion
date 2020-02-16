
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
// 头像
#define AVATAR1_PATH "avatar.jpg"
#define AVATAR2_PATH "avatar2.jpg"

void init(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS2
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
	pyrA2.allocatedFlag = 3; pyrA2.rows = height2; pyrA2.cols = width2; pyrA2.size = height2 * width2;

	pyrB0.allocatedFlag = 3; pyrB0.rows = height0; pyrB0.cols = width0; pyrB0.size = height0 * width0;
	pyrB1.allocatedFlag = 3; pyrB1.rows = height1; pyrB1.cols = width1; pyrB1.size = height1 * width1;
	pyrB2.allocatedFlag = 3; pyrB2.rows = height2; pyrB2.cols = width2; pyrB2.size = height2 * width2;

	pyrS0.allocatedFlag = 3; pyrS0.rows = height0; pyrS0.cols = width0; pyrS0.size = height0 * width0;
	pyrS1.allocatedFlag = 3; pyrS1.rows = height1; pyrS1.cols = width1; pyrS1.size = height1 * width1;
	pyrS2.allocatedFlag = 3; pyrS2.rows = height2; pyrS2.cols = width2; pyrS2.size = height2 * width2;
}

// 构造高斯金字塔
void buildG(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyr0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyr1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyr2
		)
{
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyr0, pyr1);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyr1, pyr2);
}

// 图像A拉普拉斯金字塔  因为xfopencv接口的原因不能dataflow
void buildL(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrG0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrG1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrG2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrL0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrL1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrL2,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& tempScale1L1,
		xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1>& tempScale2L1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& tempScale1L2,
		xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1>& tempScale2L2
		)
{
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrG1, pyrG0, tempScale1L1, tempScale2L1, pyrL0);
	fusion::lapPyrUpSubLevel<HEIGHT, WIDTH>(pyrG2, pyrG1, tempScale1L2, tempScale2L2, pyrL1);
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
	// 融合 ----------------------------------------------------------
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

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> tempScale1(height0, width0);
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> tempScale2(height0*2, width0*2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1(height1, width1);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2(height2, width2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1(height1, width1);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2(height2, width2);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(height1, width1);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2(height2, width2);

	fusion::dstCopyFromSrc<HEIGHT, WIDTH>(srcA, pyrA0);
	fusion::dstCopyFromSrc<HEIGHT, WIDTH>(srcB, pyrB0);

	// 构建高斯金字塔
	buildG(pyrA0, pyrA1, pyrA2);
	buildG(pyrB0, pyrB1, pyrB2);

	// 构建拉普拉斯金字塔
	buildL(pyrA0, pyrA1, pyrA2, pyrA0, pyrA1, pyrA2, tempScale1, tempScale2, tempScale1, tempScale2);
	buildL(pyrB0, pyrB1, pyrB2, pyrB0, pyrB1, pyrB2, tempScale1, tempScale2, tempScale1, tempScale2);

	xf::imwrite("pyrA0.jpg", pyrA0);
	xf::imwrite("pyrA1.jpg", pyrA1);
	xf::imwrite("pyrA2.jpg", pyrA2);

	blend(pyrA0, pyrA1, pyrA2, pyrB0, pyrB1, pyrB2, pyrS0, pyrS1, pyrS2);

	// 累加拉普拉斯金字塔
	fusion::lapPyrUpAddLevel<HEIGHT, WIDTH>(pyrS2, pyrS1, tempScale1, tempScale2, pyrS1);
	fusion::lapPyrUpAddLevel<HEIGHT, WIDTH>(pyrS1, pyrS0, tempScale1, tempScale2, pyrS0);

	// 调整亮度
	double min = 0.0;
	double max = 0.0;
	fusion::meanStdDevWrapper<HEIGHT, WIDTH>(pyrS0, min, max);
	fusion::restoreBrightness<HEIGHT, WIDTH>(pyrS0, dst, min, max);
	// blend(srcA, srcB, dst, tempScale1, tempScale2, pyrA0, pyrA1, pyrA2, pyrB0, pyrB1, pyrB2, pyrS0, pyrS1, pyrS2);
	xf::imwrite("blend.jpg", dst);
}

int main (int argc, char** argv)
{
	testBlend();
}
