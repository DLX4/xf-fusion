
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

// Í·Ïñ
#define AVATAR1_PATH "avatar.jpg"
#define AVATAR2_PATH "avatar2.jpg"

int main (int argc, char** argv)
{
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
	// ÈÚºÏ
	blend(imageA, imageB, dst);
	// xf::imwrite("blend.jpg", dst);
}
