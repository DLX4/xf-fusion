
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


static unsigned char pyrA0Data [HEIGHT * WIDTH] = {0};
static unsigned char pyrA1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char pyrA2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char pyrA3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char pyrA4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char pyrB0Data [HEIGHT * WIDTH] = {0};
static unsigned char pyrB1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char pyrB2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char pyrB3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char pyrB4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char pyrS0Data [HEIGHT * WIDTH] = {0};
static unsigned char pyrS1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char pyrS2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char pyrS3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char pyrS4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char tempA0Data [HEIGHT * WIDTH] = {0};
static unsigned char tempA1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char tempA2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char tempA3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char tempA4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char tempB0Data [HEIGHT * WIDTH] = {0};
static unsigned char tempB1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char tempB2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char tempB3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char tempB4Data [HEIGHT/16 * WIDTH/16] = {0};

static unsigned char tempS0Data [HEIGHT * WIDTH] = {0};
static unsigned char tempS1Data [HEIGHT/2 * WIDTH/2] = {0};
static unsigned char tempS2Data [HEIGHT/4 * WIDTH/4] = {0};
static unsigned char tempS3Data [HEIGHT/8 * WIDTH/8] = {0};
static unsigned char tempS4Data [HEIGHT/16 * WIDTH/16] = {0};

void testBlend() {
	cv::Mat srcA_RGB = cv::imread(AVATAR1_PATH);
	cv::Mat srcB_RGB = cv::imread(AVATAR2_PATH);

	cv::Mat srcA;
	cv::Mat srcB;
	cv::Mat output;
	cv::cvtColor(srcA_RGB, srcA, CV_RGB2GRAY);
	cv::cvtColor(srcB_RGB, srcB, CV_RGB2GRAY);
	output.create(srcA.rows, srcA.cols, CV_8UC1);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> imageA(srcA.rows, srcA.cols);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> imageB(srcB.rows, srcB.cols);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> dst(srcA.rows, srcA.cols);

	imageA.copyTo(srcA.data);
	imageB.copyTo(srcB.data);
	// 融合
	blend(imageA, imageB, dst);
	xf::imwrite("blend.jpg", dst);
}

void testBuildLaplacianPyramids() {

	cv::Mat imageA_RGB = cv::imread(AVATAR1_PATH);
	cv::Mat imageB_RGB = cv::imread(AVATAR2_PATH);

	cv::Mat imageA;
	cv::Mat imageB;
	cv::Mat output;
	cv::cvtColor(imageA_RGB, imageA, CV_RGB2GRAY);
	cv::cvtColor(imageB_RGB, imageB, CV_RGB2GRAY);
	output.create(imageA.rows, imageA.cols, CV_8UC1);

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> srcA(imageA.rows, imageA.cols);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> srcB(imageA.rows, imageA.cols);
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> dst(imageA.rows, imageA.cols);

	srcA.copyTo(imageA.data);
	srcB.copyTo(imageB.data);

	xf::imwrite("srcA.jpg", srcA);

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

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0(height0, width0, (void *)pyrA0Data);pyrA0.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1(height1, width1, (void *)pyrA1Data);pyrA1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2(height2, width2, (void *)pyrA2Data);pyrA2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA3(height3, width3, (void *)pyrA3Data);pyrA3.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA4(height4, width4, (void *)pyrA4Data);pyrA4.allocatedFlag = 3;
	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA0TempScale1(height0, width0, (void *)tempA0Data); pyrA0TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA1TempScale1(height1, width1, (void *)tempA1Data); pyrA1TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA2TempScale1(height2, width2, (void *)tempA2Data); pyrA2TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA3TempScale1(height3, width3, (void *)tempA3Data); pyrA3TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrA4TempScale1(height4, width4, (void *)tempA4Data); pyrA4TempScale1.allocatedFlag = 3;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA0TempScale2(height0, width0, (void *)tempA0Data); pyrA0TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA1TempScale2(height1, width1, (void *)tempA1Data); pyrA1TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA2TempScale2(height2, width2, (void *)tempA2Data); pyrA2TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA3TempScale2(height3, width3, (void *)tempA3Data); pyrA3TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrA4TempScale2(height4, width4, (void *)tempA4Data); pyrA4TempScale2.allocatedFlag = 3;

	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0(height0, width0, (void *)pyrB0Data); pyrB0.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1(height1, width1, (void *)pyrB1Data); pyrB1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2(height2, width2, (void *)pyrB2Data); pyrB2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB3(height3, width3, (void *)pyrB3Data); pyrB3.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB4(height4, width4, (void *)pyrB4Data); pyrB4.allocatedFlag = 3;

	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB0TempScale1(height0, width0, (void *)tempB0Data); pyrB0TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB1TempScale1(height1, width1, (void *)tempB1Data); pyrB1TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB2TempScale1(height2, width2, (void *)tempB2Data); pyrB2TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB3TempScale1(height3, width3, (void *)tempB3Data); pyrB3TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrB4TempScale1(height4, width4, (void *)tempB4Data); pyrB4TempScale1.allocatedFlag = 3;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB0TempScale2(height0, width0, (void *)tempB0Data); pyrB0TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB1TempScale2(height1, width1, (void *)tempB1Data); pyrB1TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB2TempScale2(height2, width2, (void *)tempB2Data); pyrB2TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB3TempScale2(height3, width3, (void *)tempB3Data); pyrB3TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrB4TempScale2(height4, width4, (void *)tempB4Data); pyrB4TempScale2.allocatedFlag = 3;


	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0(height0, width0, (void *)pyrS0Data); pyrS0.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1(height1, width1, (void *)pyrS1Data); pyrS1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2(height2, width2, (void *)pyrS2Data); pyrS2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS3(height3, width3, (void *)pyrS3Data); pyrS3.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS4(height4, width4, (void *)pyrS4Data); pyrS4.allocatedFlag = 3;

	// buffer scale 1
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS0TempScale1(height0, width0, (void *)tempS0Data); pyrS0TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS1TempScale1(height1, width1, (void *)tempS1Data); pyrS1TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS2TempScale1(height2, width2, (void *)tempS2Data); pyrS2TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS3TempScale1(height3, width3, (void *)tempS3Data); pyrS3TempScale1.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> pyrS4TempScale1(height4, width4, (void *)tempS4Data); pyrS4TempScale1.allocatedFlag = 3;
	// buffer scale 2
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS0TempScale2(height0, width0, (void *)tempS0Data); pyrS0TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS1TempScale2(height1, width1, (void *)tempS1Data); pyrS1TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS2TempScale2(height2, width2, (void *)tempS2Data); pyrS2TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS3TempScale2(height3, width3, (void *)tempS3Data); pyrS3TempScale2.allocatedFlag = 3;
	xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1> pyrS4TempScale2(height4, width4, (void *)tempS4Data); pyrS4TempScale2.allocatedFlag = 3;

	// 图像A 拉普拉斯金字塔
//	fusion::buildLaplacianPyramids<HEIGHT, WIDTH>(
//			srcA,
//			pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
//			pyrA0TempScale1, pyrA1TempScale1, pyrA2TempScale1, pyrA3TempScale1, pyrA4TempScale1,
//			pyrA0TempScale2, pyrA1TempScale2, pyrA2TempScale2, pyrA3TempScale2, pyrA4TempScale2
//			);
	pyrA0.copyTo(srcA.data);

	// 往下构造本层高斯金字塔 第1层
	// fusion::pyrDownUpDown<HEIGHT, WIDTH>(pyrA0, pyrA1, pyrA0TempScale1);
	xf::pyrDown<_TYPE, HEIGHT, WIDTH, _NPC1, true>(pyrA0, pyrA0TempScale1);
	fusion::dstCopyFromSrc<HEIGHT, WIDTH>(pyrA0TempScale1, pyrA1);
	xf::imwrite("pyrA0TempScale1_0.jpg", pyrA0TempScale1);

	xf::pyrUp<_TYPE, HEIGHT, WIDTH,  _NPC1>(pyrA1, pyrA0TempScale2);
	xf::imwrite("pyrA0_1.jpg", pyrA0);
	xf::imwrite("pyrA0TempScale2.jpg", pyrA0TempScale2);
	xf::imwrite("pyrA0TempScale1.jpg", pyrA0TempScale1);
	std::cout << pyrA0TempScale2.data << std::endl;
	std::cout << "---xxxxxxxxxx=" << pyrA0TempScale2.allocatedFlag << std::endl;
	std::cout << pyrA0TempScale1.data << std::endl;
	std::cout << "---" << std::endl;
	std::cout << (void *)tempA0Data << std::endl;

	xf::absdiff<_TYPE, HEIGHT, WIDTH, _NPC1>(pyrA0, pyrA0TempScale1, pyrA0);
	xf::imwrite("pyrA0TempScale2_1.jpg", pyrA0TempScale2);

	xf::imwrite("pyrA0.jpg", pyrA0);
	xf::imwrite("pyrA1.jpg", pyrA1);
	xf::imwrite("pyrA2.jpg", pyrA2);
	xf::imwrite("pyrA3.jpg", pyrA3);
	xf::imwrite("pyrA4.jpg", pyrA4);

	std::cout << "66666666666666666" << std::endl;
	return;
}

void testBlendLaplacianPyramids() {
//
//	cv::Mat srcA_RGB = cv::imread(AVATAR1_PATH);
//	cv::Mat srcB_RGB = cv::imread(AVATAR2_PATH);
//
//	cv::Mat srcA;
//	cv::Mat srcB;
//	cv::Mat output;
//	cv::cvtColor(srcA_RGB, srcA, CV_RGB2GRAY);
//	cv::cvtColor(srcB_RGB, srcB, CV_RGB2GRAY);
//	output.create(srcA.rows, srcA.cols, CV_8UC1);
//
//	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> imageA(srcA.rows, srcA.cols);
//	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> imageB(srcB.rows, srcB.cols);
//	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> dst(srcA.rows, srcA.cols);
//
//	imageA.copyTo(srcA.data);
//	imageB.copyTo(srcB.data);
//
//	int height0 = imageA.rows;
//	int width0 = imageA.cols;
//	int height1 = height0 / 2;
//	int width1 = width0 / 2;
//	int height2 = height1 / 2;
//	int width2 = width1 / 2;
//	int height3 = height2 / 2;
//	int width3 = width2 / 2;
//	int height4 = height3 / 2;
//	int width4 = width3 / 2;
//
//	xf::Mat<_TYPE, HEIGHT,    WIDTH,    _NPC1> pyrA0(height0, width0);
//	xf::Mat<_TYPE, HEIGHT/2,  WIDTH/2,  _NPC1> pyrA1(height1, width1);
//	xf::Mat<_TYPE, HEIGHT/4,  WIDTH/4,  _NPC1> pyrA2(height2, width2);
//	xf::Mat<_TYPE, HEIGHT/8,  WIDTH/8,  _NPC1> pyrA3(height3, width3);
//	xf::Mat<_TYPE, HEIGHT/16, WIDTH/16, _NPC1> pyrA4(height4, width4);
//
//	xf::Mat<_TYPE, HEIGHT,    WIDTH,    _NPC1> pyrB0(height0, width0);
//	xf::Mat<_TYPE, HEIGHT/2,  WIDTH/2,  _NPC1> pyrB1(height1, width1);
//	xf::Mat<_TYPE, HEIGHT/4,  WIDTH/4,  _NPC1> pyrB2(height2, width2);
//	xf::Mat<_TYPE, HEIGHT/8,  WIDTH/8,  _NPC1> pyrB3(height3, width3);
//	xf::Mat<_TYPE, HEIGHT/16, WIDTH/16, _NPC1> pyrB4(height4, width4);
//
//	xf::Mat<_TYPE, HEIGHT,    WIDTH,    _NPC1> pyrS0(height0, width0);
//	xf::Mat<_TYPE, HEIGHT/2,  WIDTH/2,  _NPC1> pyrS1(height1, width1);
//	xf::Mat<_TYPE, HEIGHT/4,  WIDTH/4,  _NPC1> pyrS2(height2, width2);
//	xf::Mat<_TYPE, HEIGHT/8,  WIDTH/8,  _NPC1> pyrS3(height3, width3);
//	xf::Mat<_TYPE, HEIGHT/16, WIDTH/16, _NPC1> pyrS4(height4, width4);
//
//	// 图像A 拉普拉斯金字塔
//	buildLaplacianPyramids(imageA, pyrA0, pyrA1, pyrA2, pyrA3, pyrA4);
//	xf::imwrite("pyrA0.jpg", pyrA0);
//	// 图像B 拉普拉斯金字塔
//	buildLaplacianPyramids(imageB, pyrB0, pyrB1, pyrB2, pyrB3, pyrB4);
//	xf::imwrite("pyrB0.jpg", pyrB0);
//
//	// 拉普拉斯金字塔各层分别融合 0 1 2 3 4
//	blendLaplacianPyramidsByRE2(pyrA0, pyrB0, pyrS0);
//	xf::imwrite("pyrS0.jpg", pyrS0);
//	blendLaplacianPyramidsByRE2(pyrA1, pyrB1, pyrS1);
//	xf::imwrite("pyrS1.jpg", pyrS1);
//	blendLaplacianPyramidsByRE2(pyrA2, pyrB2, pyrS2);
//	xf::imwrite("pyrS2.jpg", pyrS2);
//	blendLaplacianPyramidsByRE2(pyrA3, pyrB3, pyrS3);
//	xf::imwrite("pyrS3.jpg", pyrS3);
//	blendLaplacianPyramidsByRE2(pyrA4, pyrB4, pyrS4);
//	xf::imwrite("pyrS4.jpg", pyrS4);
//
//	// 输出图像 4 3 2 1
//	xf::Mat<_TYPE, HEIGHT/8, WIDTH/8, _NPC1> expend4(pyrS3.rows, pyrS3.cols);// upscale 2x
//	xf::pyrUp(pyrS4, expend4);
//	xf::add<XF_CONVERT_POLICY_SATURATE, _TYPE, HEIGHT/8, WIDTH/8, _NPC1>(pyrS3, expend4, pyrS3);
//
//	xf::Mat<_TYPE, HEIGHT/4, WIDTH/4, _NPC1> expend3(pyrS2.rows, pyrS2.cols);// upscale 2x
//	xf::pyrUp(pyrS3, expend3);
//	xf::add<XF_CONVERT_POLICY_SATURATE, _TYPE, HEIGHT/4, WIDTH/4, _NPC1>(pyrS2, expend3, pyrS2);
//
//	xf::Mat<_TYPE, HEIGHT/2, WIDTH/2, _NPC1> expend2(pyrS1.rows, pyrS1.cols);// upscale 2x
//	pyrUp(pyrS2, expend2);
//	xf::add<XF_CONVERT_POLICY_SATURATE, _TYPE, HEIGHT/2, WIDTH/2, _NPC1>(pyrS1, expend2, pyrS1);
//
//	xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1> expend1(pyrS0.rows, pyrS0.cols);// upscale 2x
//	pyrUp(pyrS1, expend1);
//	xf::add<XF_CONVERT_POLICY_SATURATE, _TYPE, HEIGHT, WIDTH, _NPC1>(pyrS0, expend1, pyrS0);
//
//	xf::imwrite("pyrS00.jpg", pyrS0);
//
//	// 调整亮度
//	restoreBrightness(pyrS0, dst);
//
//	xf::imwrite("dst.jpg", dst);

//	// 融合
//	blendLaplacianPyramids(pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
//						   pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
//						   pyrS0, pyrS1, pyrS2, pyrS3, pyrS4,
//						   dst);

}

int main (int argc, char** argv)
{
	testBlend();
	// testBuildLaplacianPyramids();
	//testBlendLaplacianPyramids();
	return 0;
}
