
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
// imwrite
#include "common/xf_sw_utils.h"
// 头像
#define AVATAR1_PATH "avatar.jpg"
#define AVATAR2_PATH "avatar2.jpg"

void testBlend() {
	cv::Mat srcA_RGB = cv::imread(AVATAR1_PATH);
	cv::Mat srcB_RGB = cv::imread(AVATAR2_PATH);

	cv::Mat srcA;
	cv::Mat srcB;
	cv::Mat output;
	cv::cvtColor(srcA_RGB, srcA, CV_RGB2GRAY);
	cv::cvtColor(srcB_RGB, srcB, CV_RGB2GRAY);
	output.create(srcA.rows, srcA.cols, CV_8UC1);

	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> imageA(srcA.rows, srcA.cols);
	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> imageB(srcB.rows, srcB.cols);
	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> dst(srcA.rows, srcA.cols);

	imageA.copyTo(srcA.data);
	imageB.copyTo(srcB.data);
	// 融合
	blend(imageA, imageB, dst);
	xf::imwrite("blend.jpg", dst);
}

void testBuildLaplacianPyramids() {

	cv::Mat srcA_RGB = cv::imread(AVATAR1_PATH);
	cv::Mat srcB_RGB = cv::imread(AVATAR2_PATH);

	cv::Mat srcA;
	cv::Mat srcB;
	cv::Mat output;
	cv::cvtColor(srcA_RGB, srcA, CV_RGB2GRAY);
	cv::cvtColor(srcB_RGB, srcB, CV_RGB2GRAY);
	output.create(srcA.rows, srcA.cols, CV_8UC1);

	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> imageA(srcA.rows, srcA.cols);
	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> imageB(srcB.rows, srcB.cols);
	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> dst(srcA.rows, srcA.cols);

	imageA.copyTo(srcA.data);
	imageB.copyTo(srcB.data);

	int height0 = imageA.rows;
	int width0 = imageA.cols;
	int height1 = height0 / 2;
	int width1 = width0 / 2;
	int height2 = height1 / 2;
	int width2 = width1 / 2;
	int height3 = height2 / 2;
	int width3 = width2 / 2;
	int height4 = height3 / 2;
	int width4 = width3 / 2;

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrA0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrA1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrA2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrA3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrA4(height4, width4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrB0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrB1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrB2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrB3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrB4(height4, width4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrS0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrS1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrS2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrS3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrS4(height4, width4);

	// 图像A 拉普拉斯金字塔
	buildLaplacianPyramids(imageA, pyrA0, pyrA1, pyrA2, pyrA3, pyrA4);

	xf::imwrite("pyrA0.jpg", pyrA0);
}

void testBlendLaplacianPyramids() {

	cv::Mat srcA_RGB = cv::imread(AVATAR1_PATH);
	cv::Mat srcB_RGB = cv::imread(AVATAR2_PATH);

	cv::Mat srcA;
	cv::Mat srcB;
	cv::Mat output;
	cv::cvtColor(srcA_RGB, srcA, CV_RGB2GRAY);
	cv::cvtColor(srcB_RGB, srcB, CV_RGB2GRAY);
	output.create(srcA.rows, srcA.cols, CV_8UC1);

	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> imageA(srcA.rows, srcA.cols);
	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> imageB(srcB.rows, srcB.cols);
	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> dst(srcA.rows, srcA.cols);

	imageA.copyTo(srcA.data);
	imageB.copyTo(srcB.data);

	int height0 = imageA.rows;
	int width0 = imageA.cols;
	int height1 = height0 / 2;
	int width1 = width0 / 2;
	int height2 = height1 / 2;
	int width2 = width1 / 2;
	int height3 = height2 / 2;
	int width3 = width2 / 2;
	int height4 = height3 / 2;
	int width4 = width3 / 2;

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrA0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrA1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrA2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrA3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrA4(height4, width4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrB0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrB1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrB2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrB3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrB4(height4, width4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrS0(height0, width0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrS1(height1, width1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrS2(height2, width2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrS3(height3, width3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrS4(height4, width4);

	// 图像A 拉普拉斯金字塔
	buildLaplacianPyramids(imageA, pyrA0, pyrA1, pyrA2, pyrA3, pyrA4);
	xf::imwrite("pyrA0.jpg", pyrA0);
	// 图像B 拉普拉斯金字塔
	buildLaplacianPyramids(imageB, pyrB0, pyrB1, pyrB2, pyrB3, pyrB4);
	xf::imwrite("pyrB0.jpg", pyrB0);

	// 拉普拉斯金字塔各层分别融合 0 1 2 3 4
	blendLaplacianPyramidsByRE2(pyrA0, pyrB0, pyrS0);
	xf::imwrite("pyrS0.jpg", pyrS0);
	blendLaplacianPyramidsByRE2(pyrA1, pyrB1, pyrS1);
	xf::imwrite("pyrS1.jpg", pyrS1);
	blendLaplacianPyramidsByRE2(pyrA2, pyrB2, pyrS2);
	xf::imwrite("pyrS2.jpg", pyrS2);
	blendLaplacianPyramidsByRE2(pyrA3, pyrB3, pyrS3);
	xf::imwrite("pyrS3.jpg", pyrS3);
	blendLaplacianPyramidsByRE2(pyrA4, pyrB4, pyrS4);
	xf::imwrite("pyrS4.jpg", pyrS4);

	// 输出图像 4 3 2 1
	xf::Mat<TYPE, HEIGHT/8, WIDTH/8, NPC1> expend4(pyrS3.rows, pyrS3.cols);// upscale 2x
	xf::pyrUp(pyrS4, expend4);
	xf::add<XF_CONVERT_POLICY_SATURATE, TYPE, HEIGHT/8, WIDTH/8, NPC1>(pyrS3, expend4, pyrS3);

	xf::Mat<TYPE, HEIGHT/4, WIDTH/4, NPC1> expend3(pyrS2.rows, pyrS2.cols);// upscale 2x
	xf::pyrUp(pyrS3, expend3);
	xf::add<XF_CONVERT_POLICY_SATURATE, TYPE, HEIGHT/4, WIDTH/4, NPC1>(pyrS2, expend3, pyrS2);

	xf::Mat<TYPE, HEIGHT/2, WIDTH/2, NPC1> expend2(pyrS1.rows, pyrS1.cols);// upscale 2x
	pyrUp(pyrS2, expend2);
	xf::add<XF_CONVERT_POLICY_SATURATE, TYPE, HEIGHT/2, WIDTH/2, NPC1>(pyrS1, expend2, pyrS1);

	xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> expend1(pyrS0.rows, pyrS0.cols);// upscale 2x
	pyrUp(pyrS1, expend1);
	xf::add<XF_CONVERT_POLICY_SATURATE, TYPE, HEIGHT, WIDTH, NPC1>(pyrS0, expend1, pyrS0);

	xf::imwrite("pyrS00.jpg", pyrS0);

	// 调整亮度
	restoreBrightness(pyrS0, dst);

	xf::imwrite("dst.jpg", dst);

//	// 融合
//	blendLaplacianPyramids(pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
//						   pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
//						   pyrS0, pyrS1, pyrS2, pyrS3, pyrS4,
//						   dst);

}

int main (int argc, char** argv)
{
	// testBlend();
	testBlendLaplacianPyramids();
}
