#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;
using namespace cv;

#define IMG1_PATH "D:\\code\\dan-qt\\Demo\\left.png"
#define IMG2_PATH "D:\\code\\dan-qt\\Demo\\right.png"
#define AVATAR1_PATH "D:\\code\\dan-qt\\Demo\\avatar.jpg"
#define AVATAR2_PATH "D:\\code\\dan-qt\\Demo\\avatar2.jpg"

using LapPyr = vector<Mat>;
// 通过源图像构造拉普拉斯金字塔
void buildLaplacianPyramids(Mat& src, LapPyr& pyr, int octvs=5) {
    // pyr[0~N]构成了拉普拉斯金字塔
    // 第N层（最后一层）拉普拉斯金字塔同高斯金字塔
    pyr.clear();
    pyr.resize(octvs);
    pyr[0] = src.clone();

    for (int i = 1; i < octvs; i++) {
        // 往下构造本层高斯金字塔
        Mat down;
        pyrDown(pyr[i - 1], down);
        pyr[i] = down.clone();

        // upscale 2x
        Mat expend;
        pyrUp(down, expend, pyr[i - 1].size());

        // 上一层高斯金字塔减去本层高斯金字塔*2 得到上一层拉普拉斯金字塔
        Mat subtract;
        addWeighted(pyr[i - 1], 1, expend, -1, 0, subtract);
        pyr[i - 1] = subtract.clone();
    }
}

// 顶层图像融合策略为平均梯度
void blendLaplacianPyramidsByXYDir(Mat& imageA, Mat& imageB, Mat& imageS) {
    int height = imageA.rows;
    int width = imageA.cols;

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {

            // 不检查边界
            if ((i > 1) && (i < (height - 2)) && (j > 1) && (j < (width - 2))) {
                // 3*3
                int deltaA[3] = {0};
                int deltaB[3] = {0};
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
                        for (int rgb = 0; rgb < 3; rgb++) {
                            int x = i + rowOffset;
                            int y = j + colOffset;
                            deltaA[rgb] += sqrt(0.5 * pow(imageA.at<Vec3b>(x, y)[rgb] - imageA.at<Vec3b>(x - 1, y)[rgb], 2) + 0.5 * pow(imageA.at<Vec3b>(x, y)[rgb] - imageA.at<Vec3b>(x, y - 1)[rgb], 2));
                            deltaB[rgb] += sqrt(0.5 * pow(imageB.at<Vec3b>(x, y)[rgb] - imageB.at<Vec3b>(x - 1, y)[rgb], 2) + 0.5 * pow(imageB.at<Vec3b>(x, y)[rgb] - imageB.at<Vec3b>(x, y - 1)[rgb], 2));
                        }
                    }
                }
                // 根据梯度大小进行融合
                if (deltaA[0] > deltaB[0]) {
                    imageS.at<Vec3b>(i, j)[0] = imageA.at<Vec3b>(i, j)[0];
                } else {
                    imageS.at<Vec3b>(i, j)[0] = imageB.at<Vec3b>(i, j)[0];
                }
                if (deltaA[1] > deltaB[1]) {
                    imageS.at<Vec3b>(i, j)[1] = imageA.at<Vec3b>(i, j)[1];
                } else {
                    imageS.at<Vec3b>(i, j)[1] = imageB.at<Vec3b>(i, j)[1];
                }
                if (deltaA[2] > deltaB[2]) {
                    imageS.at<Vec3b>(i, j)[2] = imageA.at<Vec3b>(i, j)[2];
                } else {
                    imageS.at<Vec3b>(i, j)[2] = imageB.at<Vec3b>(i, j)[2];
                }
            }
        }
    }
}

// 非顶层融合策略为区域能量
int G[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}
};
void blendLaplacianPyramidsByRE(Mat& imageA, Mat& imageB, Mat& imageS) {
    int height = imageA.rows;
    int width = imageB.cols;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            // 不检查边界
            if ((i > 1) && (i < (height - 2)) && (j > 1) && (j < (width - 2))) {
                // 3*3
                int deltaA[3] = {0};
                int deltaB[3] = {0};
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
                        for (int rgb = 0; rgb < 3; rgb++) {
                            int x = i + rowOffset;
                            int y = j + colOffset;

                            deltaA[rgb] += G[1+rowOffset][1+colOffset] * imageA.at<Vec3b>(x, y)[rgb];
                            deltaB[rgb] += G[1+rowOffset][1+colOffset] * imageB.at<Vec3b>(x, y)[rgb];
                        }
                    }
                }
                // 根据梯度大小进行融合
                if (deltaA[0] > deltaB[0]) {
                    imageS.at<Vec3b>(i, j)[0] = imageA.at<Vec3b>(i, j)[0];
                } else {
                    imageS.at<Vec3b>(i, j)[0] = imageB.at<Vec3b>(i, j)[0];
                }
                if (deltaA[1] > deltaB[1]) {
                    imageS.at<Vec3b>(i, j)[1] = imageA.at<Vec3b>(i, j)[1];
                } else {
                    imageS.at<Vec3b>(i, j)[1] = imageB.at<Vec3b>(i, j)[1];
                }
                if (deltaA[2] > deltaB[2]) {
                    imageS.at<Vec3b>(i, j)[2] = imageA.at<Vec3b>(i, j)[2];
                } else {
                    imageS.at<Vec3b>(i, j)[2] = imageB.at<Vec3b>(i, j)[2];
                }
            }
        }
    }
}

// 将两个原图像的拉普拉斯金字塔融合
void blendLaplacianPyramids(LapPyr& pyrA, LapPyr& pyrB, LapPyr& pyrS, Mat& dst) {
    pyrS.clear();
    pyrS.resize(pyrA.size());

    // 拉普拉斯金字塔各层分别融合
    for (int idx = 0; idx < pyrS.size(); idx++) {
        pyrS[idx] = pyrA[idx].clone();
        if (idx == pyrS.size() - 1) {
            blendLaplacianPyramidsByXYDir(pyrA[idx], pyrB[idx], pyrS[idx]);
        } else {
            blendLaplacianPyramidsByRE(pyrA[idx], pyrB[idx], pyrS[idx]);
        }
        // blendLaplacianPyramidsByRE(pyrA[idx], pyrB[idx], pyrS[idx]);
    }

    // 输出图像
    for (int i = pyrS.size() - 1; i >= 1; i--) {
        // upscale 2x
        Mat expend;
        pyrUp(pyrS[i], expend, pyrS[i - 1].size());
        // resize(pyrS[i], expend, Size(pyrS[i-1].cols, pyrS[i-1].rows), 0, 0, INTER_CUBIC);

        Mat add;
        addWeighted(pyrS[i - 1], 1, expend, 1, 0, add);
        pyrS[i - 1] = add.clone();
    }
    dst = pyrS[0].clone();
}

void showLaplacianPyramids(LapPyr& pyr) {
    for(int i = 0; i < pyr.size(); i++) {
        imshow("88", pyr[i]);
        waitKey(0);
    }
}

int main()
{
    LapPyr LA;
    LapPyr LB;
    LapPyr LS;
    Mat dst;

    // AVATAR_PATH IMG1_PATH

    // 图像A 拉普拉斯金字塔
    Mat srcA = imread(IMG1_PATH);
    buildLaplacianPyramids(srcA, LA);
    // showLaplacianPyramids(LA);

    // 图像B 拉普拉斯金字塔
    Mat srcB = imread(IMG2_PATH);
    buildLaplacianPyramids(srcB, LB);
    // showLaplacianPyramids(LB);

    // 融合
    blendLaplacianPyramids(LA, LB, LS, dst);
    imshow("success", dst);
    waitKey(0);

    return 0;
}
