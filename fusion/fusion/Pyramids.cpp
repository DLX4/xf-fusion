#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;
// 路灯监控
#define IMG011_PATH "..\\image\\01b.jpg"
#define IMG012_PATH "..\\image\\01a.jpg"
// 人在山里
#define IMG11_PATH "..\\image\\test1.bmp"
#define IMG12_PATH "..\\image\\test2.bmp"
// 电线杆
#define IMG21_PATH "..\\image\\left.png"
#define IMG22_PATH "..\\image\\right.png"
// 烟雾弹 士兵
#define IMG31_PATH "..\\image\\test31.bmp"
#define IMG32_PATH "..\\image\\test32.bmp"
// 老照片
#define IMGTEMP1_PATH "..\\image\\temp1.jpg"
#define IMGTEMP2_PATH "..\\image\\temp2.jpg"
// 头像
#define AVATAR1_PATH "..\\image\\avatar.jpg"
#define AVATAR2_PATH "..\\image\\avatar2.jpg"

#define FLIR_00001_1 "..\\image\\FLIR_video_00001.jpg"
#define FLIR_00001_2 "..\\image\\FLIR_video_00001.jpeg"
// 将mat输出到文件，便于调试
void writeMatToFile(Mat& m, const char* filename) {
    ofstream fout(filename);

    if (!fout) {
        std::cout << "File Not Opened" << std::endl;
        return;
    }

    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            fout << m.at<uchar>(i, j) << "\t";
        }
        fout << std::endl;
    }
    fout.close();
}

// 浮点图像mat输出到文件，便于调试
void writeMatToFile2(Mat& m, const char* filename) {
    ofstream fout(filename);

    if (!fout) {
        std::cout << "File Not Opened" << std::endl;
        return;
    }

    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            fout << m.at<Vec3f>(i, j) << "\t";
        }
        fout << std::endl;
    }
    fout.close();
}

// 计算并打印图片的亮度
void showBrightness(Mat& image) {
    double brightness = 0.0;
    // cvtColor(image, gray, CV_RGB2GRAY);
    Scalar scalar = mean(image);
    brightness = scalar.val[0];

    std::cout << "brightnes=" << brightness << std::endl;
}
#define DYNAMIC 2
// 重新调整图片亮度
void restoreBrightness(Mat& src, Mat& dst) {
    // 归一化
    // 分通道分别计算均值，均方差
    double means = 0.0;
    double sds = 0.0;
    double mins = 0.0;
    double maxs = 0.0;

    Mat tmpMean;
    Mat tmpSd;
    meanStdDev(src, tmpMean, tmpSd);
    means = tmpMean.at<double>(0,0);
    sds = tmpSd.at<double>(0,0);
    mins = means - DYNAMIC * sds;
    maxs = means + DYNAMIC * sds;

    std::cout << "means= " << means << "; sds= " << sds << std::endl;

    for (int i = 0; i < dst.rows; i++) {
        for(int j = 0; j < dst.cols; j++) {
            double value = (src.at<uchar>(i, j) - mins) * 255 / (maxs - mins);

            if ( value > 255) {
                dst.at<uchar>(i, j) = 255;
            } else if (value < 0) {
                dst.at<uchar>(i, j) = 0;
            } else {
                dst.at<uchar>(i, j) = (uchar)value;
            }
        }
    }
}

// 通过源图像构造拉普拉斯金字塔 (注意：图像的长宽需要被16整除)
void buildLaplacianPyramids(Mat& src, Mat& pyr0, Mat& pyr1, Mat& pyr2, Mat& pyr3, Mat& pyr4) {
    pyr0 = src;

    // 往下构造本层高斯金字塔 第1层
    pyrDown(pyr0, pyr1);
    // 往下构造本层高斯金字塔 第2层
    pyrDown(pyr1, pyr2);
    // 往下构造本层高斯金字塔 第3层
    pyrDown(pyr2, pyr3);
    // 往下构造本层高斯金字塔 第4层
    pyrDown(pyr3, pyr4);

    // 上一层高斯金字塔减去本层高斯金字塔*2得到上一层拉普拉斯金字塔；pyr[0~N]构成了拉普拉斯金字塔；第N层（最后一层）拉普拉斯金字塔同高斯金字塔
    // 第1层
    Mat expend1;// upscale 2x
    pyrUp(pyr1, expend1, pyr0.size());
    addWeighted(pyr0, 1, expend1, -1, 0, pyr0);

    // 第2层
    Mat expend2;// upscale 2x
    pyrUp(pyr2, expend2, pyr1.size());
    addWeighted(pyr1, 1, expend2, -1, 0, pyr1);

    // 第3层
    Mat expend3;// upscale 2x
    pyrUp(pyr3, expend3, pyr2.size());
    addWeighted(pyr2, 1, expend3, -1, 0, pyr2);

    // 第4层
    Mat expend4;// upscale 2x
    pyrUp(pyr4, expend4, pyr3.size());
    addWeighted(pyr3, 1, expend4, -1, 0, pyr3);
}


// 融合策略为区域能量
void blendLaplacianPyramidsByRE2(Mat& imageA, Mat& imageB, Mat& imageS) {
    // 均值滤波
    double G[3][3] = {
        {0.1111, 0.1111, 0.1111},
        {0.1111, 0.1111, 0.1111},
        {0.1111, 0.1111, 0.1111}
    };
    double matchDegreeLimit = 0.618;

    int height = imageA.rows;
    int width = imageB.cols;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            // 不检查边界
            if ((i > 1) && (i < (height - 2)) && (j > 1) && (j < (width - 2))) {
                // 3*3
                double deltaA = 0.0;
                double deltaB = 0.0;
                double matchDegree = 0.0;
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
                        // 单通道
                        int x = i + rowOffset;
                        int y = j + colOffset;

                        deltaA += G[1+rowOffset][1+colOffset] * pow(imageA.at<uchar>(x, y), 2);
                        deltaB += G[1+rowOffset][1+colOffset] * pow(imageB.at<uchar>(x, y), 2);
                        matchDegree += G[1+rowOffset][1+colOffset] * imageA.at<uchar>(x, y) * imageB.at<uchar>(x, y);
                    }
                }
                // 计算匹配度
                matchDegree = pow(matchDegree, 2) / (deltaA * deltaB);

                if (isnan(matchDegree) || matchDegree < matchDegreeLimit) {
                    if (deltaA == deltaB) {
                        imageS.at<uchar>(i, j) = 0.5 * imageA.at<uchar>(i, j) + 0.5 * imageB.at<uchar>(i, j);
                    } else if (deltaA > deltaB) {
                        imageS.at<uchar>(i, j) = imageA.at<uchar>(i, j);
                    } else {
                        imageS.at<uchar>(i, j) = imageB.at<uchar>(i, j);
                    }
                } else {
                    double wMin = 0.5 * (1 - (1 - matchDegree)/(1 - matchDegreeLimit));
                    imageS.at<uchar>(i, j) = min(imageA.at<uchar>(i, j), imageB.at<uchar>(i, j)) * wMin + max(imageA.at<uchar>(i, j), imageB.at<uchar>(i, j)) * (1 - wMin);
                }
            } else {
                // 边界55开填充
                imageS.at<uchar>(i, j) = 0.5 * imageA.at<uchar>(i, j) + 0.5 * imageB.at<uchar>(i, j);
            }
        }
    }
}


// 将两个原图像的拉普拉斯金字塔融合
void blendLaplacianPyramids(Mat& pyrA0, Mat& pyrA1, Mat& pyrA2, Mat& pyrA3, Mat& pyrA4,
                            Mat& pyrB0, Mat& pyrB1, Mat& pyrB2, Mat& pyrB3, Mat& pyrB4,
                            Mat& pyrS0, Mat& pyrS1, Mat& pyrS2, Mat& pyrS3, Mat& pyrS4,
                            Mat& dst) {

    // 拉普拉斯金字塔各层分别融合 0 1 2 3 4
    blendLaplacianPyramidsByRE2(pyrA0, pyrB0, pyrS0);
    blendLaplacianPyramidsByRE2(pyrA1, pyrB1, pyrS1);
    blendLaplacianPyramidsByRE2(pyrA2, pyrB2, pyrS2);
    blendLaplacianPyramidsByRE2(pyrA3, pyrB3, pyrS3);
    blendLaplacianPyramidsByRE2(pyrA4, pyrB4, pyrS4);

    // 输出图像 4 3 2 1
    Mat expend4;// upscale 2x
    pyrUp(pyrS4, expend4, pyrS3.size());
    addWeighted(pyrS3, 1, expend4, 1, 0, pyrS3);

    Mat expend3;// upscale 2x
    pyrUp(pyrS3, expend3, pyrS2.size());
    addWeighted(pyrS2, 1, expend3, 1, 0, pyrS2);

    Mat expend2;// upscale 2x
    pyrUp(pyrS2, expend2, pyrS1.size());
    addWeighted(pyrS1, 1, expend2, 1, 0, pyrS1);

    Mat expend1;// upscale 2x
    pyrUp(pyrS1, expend1, pyrS0.size());
    addWeighted(pyrS0, 1, expend1, 1, 0, pyrS0);

    // 调整亮度
    restoreBrightness(pyrS0, dst);
}

//void showLaplacianPyramids(LapPyr& pyr) {
//    for(int i = 0; i < pyr.size(); i++) {
//        imshow("88", pyr[i]);
//        waitKey(0);
//    }
//}

// 图像融合顶层函数
void blend(Mat& srcA, Mat& srcB, Mat& dst) {
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

    Mat pyrA0 = Mat::zeros(Size(width0, height0), CV_8UC1);
    Mat pyrA1 = Mat::zeros(Size(width1, height1), CV_8UC1);
    Mat pyrA2 = Mat::zeros(Size(width2, height2), CV_8UC1);
    Mat pyrA3 = Mat::zeros(Size(width3, height3), CV_8UC1);
    Mat pyrA4 = Mat::zeros(Size(width4, height4), CV_8UC1);

    Mat pyrB0 = Mat::zeros(Size(width0, height0), CV_8UC1);
    Mat pyrB1 = Mat::zeros(Size(width1, height1), CV_8UC1);
    Mat pyrB2 = Mat::zeros(Size(width2, height2), CV_8UC1);
    Mat pyrB3 = Mat::zeros(Size(width3, height3), CV_8UC1);
    Mat pyrB4 = Mat::zeros(Size(width4, height4), CV_8UC1);

    Mat pyrS0 = Mat::zeros(Size(width0, height0), CV_8UC1);
    Mat pyrS1 = Mat::zeros(Size(width1, height1), CV_8UC1);
    Mat pyrS2 = Mat::zeros(Size(width2, height2), CV_8UC1);
    Mat pyrS3 = Mat::zeros(Size(width3, height3), CV_8UC1);
    Mat pyrS4 = Mat::zeros(Size(width4, height4), CV_8UC1);

    // 图像A 拉普拉斯金字塔

    buildLaplacianPyramids(srcA, pyrA0, pyrA1, pyrA2, pyrA3, pyrA4);

    // 图像B 拉普拉斯金字塔
    buildLaplacianPyramids(srcB, pyrB0, pyrB1, pyrB2, pyrB3, pyrB4);

    // 融合
    blendLaplacianPyramids(pyrA0, pyrA1, pyrA2, pyrA3, pyrA4,
                           pyrB0, pyrB1, pyrB2, pyrB3, pyrB4,
                           pyrS0, pyrS1, pyrS2, pyrS3, pyrS4,
                           dst);
}

int main() {
    // 图像长宽被16整除 灰度图（如果不是需要cvtColor(image, gray, CV_RGB2GRAY);）
    Mat srcA_RGB = imread(AVATAR1_PATH);
    Mat srcB_RGB = imread(AVATAR2_PATH);

    Mat srcA;
    Mat srcB;
    cvtColor(srcA_RGB, srcA, CV_RGB2GRAY);
    cvtColor(srcB_RGB, srcB, CV_RGB2GRAY);
    // 融合
    Mat dst1 = Mat::zeros(srcA.size(), CV_8UC1);
    blend(srcA, srcB, dst1);

    imshow("11", dst1);

    showBrightness(dst1);

    waitKey(0);

    // writeMatToFile(dst1, "..\\dst1.txt");
    // writeMatToFile(dst2, "..\\dst2.txt");
    // writeMatToFile(dst3, "..\\dst3.txt");

    return 0;
}
