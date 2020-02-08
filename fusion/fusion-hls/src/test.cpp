
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include "ap_int.h"
#include "hls_stream.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
using LapPyr = vector<Mat>;

#define IMG11_PATH "test1.bmp"
#define IMG12_PATH "test2.bmp"

int main (int argc, char** argv)
{
    LapPyr LA;
    LapPyr LB;
    LapPyr LS;

    // AVATAR_PATH IMG1_PATH

    // 图像A 拉普拉斯金字塔
    Mat srcA = imread(IMG11_PATH);
    Mat srcASSR = Mat::zeros(srcA.size(), CV_8UC3);
    // ssr(srcA, srcASSR, 15);
    msr(srcA, srcASSR, {15, 80, 250});
    imshow("msr", srcASSR);
    buildLaplacianPyramids(srcASSR, LA);
    // showLaplacianPyramids(LA);

    // 图像B 拉普拉斯金字塔
    Mat srcB = imread(IMG12_PATH);
    buildLaplacianPyramids(srcB, LB);
    // showLaplacianPyramids(LB);

    // 融合
    Mat dst1;
    blendLaplacianPyramids(LA, LB, LS, dst1, 1);
    restoreBrightness(dst1);
    imshow("1", dst1);

    Mat dst2;
    blendLaplacianPyramids(LA, LB, LS, dst2, 2);
    restoreBrightness(dst2);
    imshow("2", dst2);

    Mat dst3;
    blendLaplacianPyramids(LA, LB, LS, dst3, 3);
    restoreBrightness(dst3);
    imshow("3", dst3);

    Mat dst4;
    blendLaplacianPyramids(LA, LB, LS, dst4, 4);
    restoreBrightness(dst4);
    imshow("4", dst4);
}
