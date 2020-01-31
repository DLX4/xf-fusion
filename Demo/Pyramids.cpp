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

#define IMG1_PATH "D:\\code\\dan-qt\\Demo\\test1.bmp"
#define IMG2_PATH "D:\\code\\dan-qt\\Demo\\test2.bmp"
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
                for (int rgb = 0; rgb < 3; rgb++) {
                    if (deltaA[rgb] == deltaB[rgb]) {
                        imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                    } else if (deltaA[rgb] > deltaB[rgb]) {
                        imageS.at<Vec3b>(i, j)[rgb] = imageA.at<Vec3b>(i, j)[rgb];
                    } else {
                        imageS.at<Vec3b>(i, j)[rgb] = imageB.at<Vec3b>(i, j)[rgb];
                    }
                }

            } else {
                // 边界55开填充
                for (int rgb = 0; rgb < 3; rgb++) {
                    imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                }
            }
        }
    }
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
                double deltaA[3] = {0.0};
                double deltaB[3] = {0.0};
                double matchDegree[3] = {0.0};
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
                        for (int rgb = 0; rgb < 3; rgb++) {
                            int x = i + rowOffset;
                            int y = j + colOffset;

                            deltaA[rgb] += G[1+rowOffset][1+colOffset] * pow(imageA.at<Vec3b>(x, y)[rgb], 2);
                            deltaB[rgb] += G[1+rowOffset][1+colOffset] * pow(imageB.at<Vec3b>(x, y)[rgb], 2);
                            matchDegree[rgb] += G[1+rowOffset][1+colOffset] * imageA.at<Vec3b>(x, y)[rgb] * imageB.at<Vec3b>(x, y)[rgb];
                        }
                    }
                }
                // 计算匹配度
                for (int rgb = 0; rgb < 3; rgb++) {
                    matchDegree[rgb] = pow(matchDegree[rgb], 2) / (deltaA[rgb] * deltaB[rgb]);

                    if (isnan(matchDegree[rgb]) || matchDegree[rgb] < matchDegreeLimit) {
                        if (deltaA[rgb] == deltaB[rgb]) {
                            imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                        } else if (deltaA[rgb] > deltaB[rgb]) {
                            imageS.at<Vec3b>(i, j)[rgb] = imageA.at<Vec3b>(i, j)[rgb];
                        } else {
                            imageS.at<Vec3b>(i, j)[rgb] = imageB.at<Vec3b>(i, j)[rgb];
                        }
                    } else {
                        double wMin = 0.5 * (1 - (1 - matchDegree[rgb])/(1 - matchDegreeLimit));
                        imageS.at<Vec3b>(i, j)[rgb] = min(imageA.at<Vec3b>(i, j)[rgb], imageB.at<Vec3b>(i, j)[rgb]) * wMin + max(imageA.at<Vec3b>(i, j)[rgb], imageB.at<Vec3b>(i, j)[rgb]) * (1 - wMin);
                    }
                }
            } else {
                // 边界55开填充
                for (int rgb = 0; rgb < 3; rgb++) {
                    imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                }
            }
        }
    }
}

// 融合策略为区域能量
void blendLaplacianPyramidsByRE1(Mat& imageA, Mat& imageB, Mat& imageS) {
    int G[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };
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
                for (int rgb = 0; rgb < 3; rgb++) {
                    if (deltaA[rgb] == deltaB[rgb]) {
                        imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                    } else if (deltaA[rgb] > deltaB[rgb]) {
                        imageS.at<Vec3b>(i, j)[rgb] = imageA.at<Vec3b>(i, j)[rgb];
                    } else {
                        imageS.at<Vec3b>(i, j)[rgb] = imageB.at<Vec3b>(i, j)[rgb];
                    }
                }
            } else {
                // 边界55开填充
                for (int rgb = 0; rgb < 3; rgb++) {
                    imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                }
            }
        }
    }
}

// 将两个原图像的拉普拉斯金字塔融合
void blendLaplacianPyramids(LapPyr& pyrA, LapPyr& pyrB, LapPyr& pyrS, Mat& dst, int strategy) {
    pyrS.clear();
    pyrS.resize(pyrA.size());

    // 拉普拉斯金字塔各层分别融合
    for (int idx = 0; idx < pyrS.size(); idx++) {
        pyrS[idx] = Mat::zeros(pyrA[idx].size(), pyrA[idx].type());
        switch(strategy) {
        case 1:
            if (idx == pyrS.size() - 1) {
                blendLaplacianPyramidsByXYDir(pyrA[idx], pyrB[idx], pyrS[idx]);
            } else {
                blendLaplacianPyramidsByRE2(pyrA[idx], pyrB[idx], pyrS[idx]);
            }
            break;
        case 2:
            blendLaplacianPyramidsByRE1(pyrA[idx], pyrB[idx], pyrS[idx]);
            break;
        case 3:
            blendLaplacianPyramidsByRE2(pyrA[idx], pyrB[idx], pyrS[idx]);
            break;
        default:
            break;
        }
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

// 将mat输出到文件
void writeMatToFile(Mat& m, const char* filename) {
    ofstream fout(filename);

    if (!fout) {
        std::cout << "File Not Opened" << std::endl;
        return;
    }

    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            fout << m.at<Vec3b>(i, j) << "\t";
        }
        fout << std::endl;
    }
    fout.close();
}

int main() {
    LapPyr LA;
    LapPyr LB;
    LapPyr LS;

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
    Mat dst1;
    blendLaplacianPyramids(LA, LB, LS, dst1, 1);
    imshow("1", dst1);

    Mat dst2;
    blendLaplacianPyramids(LA, LB, LS, dst2, 2);
    imshow("2", dst2);

    Mat dst3;
    blendLaplacianPyramids(LA, LB, LS, dst3, 3);
    imshow("3", dst3);
    waitKey(0);

    // writeMatToFile(dst1, "D:\\code\\dan-qt\\Demo\\dst1.txt");
    // writeMatToFile(dst2, "D:\\code\\dan-qt\\Demo\\dst2.txt");
    // writeMatToFile(dst3, "D:\\code\\dan-qt\\Demo\\dst3.txt");

    return 0;
}
