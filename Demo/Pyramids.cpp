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
#define IMG011_PATH "D:\\code\\dan-qt\\Demo\\01b.jpg"
#define IMG012_PATH "D:\\code\\dan-qt\\Demo\\01a.jpg"

#define IMG11_PATH "D:\\code\\dan-qt\\Demo\\test1.bmp"
#define IMG12_PATH "D:\\code\\dan-qt\\Demo\\test2.bmp"
#define IMG21_PATH "D:\\code\\dan-qt\\Demo\\left.png"
#define IMG22_PATH "D:\\code\\dan-qt\\Demo\\right.png"

#define IMG31_PATH "D:\\code\\dan-qt\\Demo\\test31.bmp"
#define IMG32_PATH "D:\\code\\dan-qt\\Demo\\test32.bmp"

#define AVATAR1_PATH "D:\\code\\dan-qt\\Demo\\avatar.jpg"
#define AVATAR2_PATH "D:\\code\\dan-qt\\Demo\\avatar2.jpg"
// 将mat输出到文件，便于调试
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

// 线性运算 ax+b
void linearTransform(Mat& image, int a, int b) {
    int height = image.rows;
    int width = image.cols;

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            for (int rgb = 0; rgb < 3; rgb++) {
                image.at<Vec3f>(i, j)[rgb] = a * image.at<Vec3f>(i, j)[rgb] + b;
            }
        }
    }
}

// 伽马变换
#define THRESHOLD_GAMMA 128
void gamma(Mat& imageFrom, Mat& imageTo) {
    // 计算平均灰度
    Mat gray;
    double brightness = 0.0;
    cvtColor(imageFrom, gray, CV_RGB2GRAY);
    Scalar scalar = mean(gray);
    brightness = scalar.val[0];

    std::cout << "brightnes--------" << std::endl;
    std::cout << brightness << std::endl;

    if (brightness > THRESHOLD_GAMMA) {
        // 伽马变换对于图像对比度偏低，并且整体亮度值偏高（对于于相机过曝）情况下的图像增强效果明显。
        Mat imageGamma(imageFrom.size(), CV_32FC3);
        for (int i = 0; i < imageFrom.rows; i++) {
            for (int j = 0; j < imageFrom.cols; j++) {
                imageGamma.at<Vec3f>(i, j)[0] = (imageFrom.at<Vec3b>(i, j)[0])*(imageFrom.at<Vec3b>(i, j)[0])*(imageFrom.at<Vec3b>(i, j)[0]);
                imageGamma.at<Vec3f>(i, j)[1] = (imageFrom.at<Vec3b>(i, j)[1])*(imageFrom.at<Vec3b>(i, j)[1])*(imageFrom.at<Vec3b>(i, j)[1]);
                imageGamma.at<Vec3f>(i, j)[2] = (imageFrom.at<Vec3b>(i, j)[2])*(imageFrom.at<Vec3b>(i, j)[2])*(imageFrom.at<Vec3b>(i, j)[2]);
            }
        }
        // 归一化到0~255
        normalize(imageGamma, imageGamma, 0, 255, CV_MINMAX);
        // 转换成8bit图像显示
        convertScaleAbs(imageGamma, imageTo);
    }
}

//  将ssr msr处理过的图像从对数空间转到实数空间（线性运算）  x*128 + 128
void convertTo8UC3Way1(Mat& imageFrom, Mat& imageTo) {
    int height = imageFrom.rows;
    int width = imageFrom.cols;

    linearTransform(imageFrom, 128, 128);

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            for (int rgb = 0; rgb < 3; rgb++) {
                // double value = (imageFrom.at<Vec3f>(i, j)[rgb] - min) * 255 / (max - min);
                double value = (imageFrom.at<Vec3f>(i, j)[rgb]);

                if ( value > 255) {
                    imageTo.at<Vec3b>(i, j)[rgb] = 255;
                } else if (value < 0) {
                    imageTo.at<Vec3b>(i, j)[rgb] = 0;
                } else {
                    imageTo.at<Vec3b>(i, j)[rgb] = (uchar)value;
                }
            }
        }
    }
}

#define THRESHOLD_LOG 1

// 将ssr msr处理过的图像从对数空间转到实数空间（归一化+过曝伽马变换）
#define DYNAMIC 2
void convertTo8UC3Way2(Mat& imageFrom, Mat& imageTo) {
    // 归一化
    // 分通道分别计算均值，均方差
    vector<Mat> channelMatrices(imageFrom.channels());
    vector<double> means(imageFrom.channels());
    vector<double> sds(imageFrom.channels());
    vector<double> mins(imageFrom.channels());
    vector<double> maxs(imageFrom.channels());

    split(imageFrom, channelMatrices);
    for ( int i = 0; i < channelMatrices.size(); i++) {
        Mat tmpMean;
        Mat tmpSd;
        Mat& channelMat = channelMatrices[i];
        meanStdDev(channelMat, tmpMean, tmpSd);
        means[i] = tmpMean.at<double>(0,0);
        sds[i] = tmpSd.at<double>(0,0);
        mins[i] = means[i] - DYNAMIC * sds[i];
        maxs[i] = means[i] + DYNAMIC * sds[i];
    }

    for (int i = 0; i < imageFrom.rows; i++) {
        for(int j = 0; j < imageFrom.cols; j++) {
            for (int rgb = 0; rgb < 3; rgb++) {
                double value = (imageFrom.at<Vec3f>(i, j)[rgb] - mins[rgb]) * 255 / (maxs[rgb] - mins[rgb]);

                if ( value > 255) {
                    imageFrom.at<Vec3f>(i, j)[rgb] = 255;
                } else if (value < 0) {
                    imageFrom.at<Vec3f>(i, j)[rgb] = 0;
                } else {
                    imageFrom.at<Vec3f>(i, j)[rgb] = (uchar)value;
                }
            }
        }
    }

    // 转换成8bit图像
    convertScaleAbs(imageFrom, imageTo);
}

//   CV_8UC3 转 CV_32FC3
void convertTo32FC3(Mat& imageFrom, Mat& imageTo) {
    int height = imageFrom.rows;
    int width = imageFrom.cols;

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            for (int rgb = 0; rgb < 3; rgb++) {
                imageTo.at<Vec3f>(i, j)[rgb] = imageFrom.at<Vec3b>(i, j)[rgb];
            }
        }
    }
}

// 对数运算前去掉0值
void killZero(Mat& image) {
    int height = image.rows;
    int width = image.cols;

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            for (int rgb = 0; rgb < 3; rgb++) {
                image.at<Vec3f>(i, j)[rgb] += THRESHOLD_LOG;
            }
        }
    }
}

// 图像增强
void ssr(Mat& src, Mat& dst, double sigma) {
    Mat srcFloat = Mat::zeros(src.size(), CV_32FC3);
    Mat srcBlur = Mat::zeros(src.size(), src.type());
    Mat srcBlurFloat = Mat::zeros(src.size(), CV_32FC3);
    Mat result = Mat::zeros(src.size(), CV_32FC3);

    // 源图像转换成浮点
    convertTo32FC3(src, srcFloat);

    // 转对数
    killZero(srcFloat);
    log(srcFloat, srcFloat);

    // 源图像滤波
    GaussianBlur(src, srcBlur, Size(0, 0), sigma);
    convertTo32FC3(srcBlur, srcBlurFloat);
    // 转对数
    killZero(srcBlurFloat);
    log(srcBlurFloat, srcBlurFloat);

    result = (srcFloat - srcBlurFloat) / log(10);
    // result = (srcFloat - srcBlurFloat);
    imshow("float", result);
    convertTo8UC3Way2(result, dst);
}

// msr
void msr(Mat& img, Mat& dst, const vector<double>& sigmas) {

    vector<Mat> matrices;
    for (size_t i = 1; i < sigmas.size(); ++i) {
        // begin ssr
        Mat srcFloat = Mat::zeros(img.size(), CV_32FC3);
        Mat srcBlur = Mat::zeros(img.size(), img.type());
        Mat srcBlurFloat = Mat::zeros(img.size(), CV_32FC3);
        Mat result = Mat::zeros(img.size(), CV_32FC3);

        // 源图像转换成浮点
        img.convertTo(srcFloat, CV_32FC3);
        // 转对数
        killZero(srcFloat);
        log(srcFloat, srcFloat);

        // 源图像滤波
        GaussianBlur(img, srcBlur, Size(0, 0), sigmas[i]);
        srcBlur.convertTo(srcBlurFloat, CV_32FC3);
        // 转对数
        killZero(srcBlurFloat);
        log(srcBlurFloat, srcBlurFloat);

        result = (srcFloat - srcBlurFloat) / log(10);
        // result = (srcFloat - srcBlurFloat);

        matrices.emplace_back(result);
    }

    Mat result = Mat::zeros(img.size(), CV_32FC3);
    result = std::accumulate(matrices.begin(), matrices.end(), result) / sigmas.size();
    convertTo8UC3Way2(result, dst);
}

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

// 顶层融合策略: 选取亮度较大的图片
void blendLaplacianPyramidsByBrightness(Mat& imageA, Mat& imageB, Mat& imageS) {
    Mat grayA;
    Mat grayB;
    double brightnessA = 0.0;
    double brightnessB = 0.0;

    cvtColor(imageA, grayA, CV_RGB2GRAY);
    Scalar scalar = mean(grayA);
    brightnessA = scalar.val[0];

    cvtColor(imageB, grayB, CV_RGB2GRAY);
    scalar = mean(grayB);
    brightnessB = scalar.val[0];

    std::cout << brightnessA << std::endl;
    std::cout << brightnessB << std::endl;

    if (brightnessA >= brightnessB) {
        imageS = imageA;
    } else {
        imageS = imageB;
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
        case 4:
            if (idx == pyrS.size() - 1) {
                blendLaplacianPyramidsByBrightness(pyrA[idx], pyrB[idx], pyrS[idx]);
            } else {
                blendLaplacianPyramidsByRE2(pyrA[idx], pyrB[idx], pyrS[idx]);
            }
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

int main() {
    LapPyr LA;
    LapPyr LB;
    LapPyr LS;

    // AVATAR_PATH IMG1_PATH

    // 图像A 拉普拉斯金字塔
    Mat srcA = imread(IMG31_PATH);
    Mat srcASSR = Mat::zeros(srcA.size(), CV_8UC3);
    // ssr(srcA, srcASSR, 15);
    msr(srcA, srcASSR, {15, 80, 250});
    imshow("msr", srcASSR);
    buildLaplacianPyramids(srcASSR, LA);
    // showLaplacianPyramids(LA);

    // 图像B 拉普拉斯金字塔
    Mat srcB = imread(IMG32_PATH);
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

    Mat dst4;
    blendLaplacianPyramids(LA, LB, LS, dst4, 4);
    imshow("4", dst4);

    waitKey(0);

    // writeMatToFile(dst1, "D:\\code\\dan-qt\\Demo\\dst1.txt");
    // writeMatToFile(dst2, "D:\\code\\dan-qt\\Demo\\dst2.txt");
    // writeMatToFile(dst3, "D:\\code\\dan-qt\\Demo\\dst3.txt");

    return 0;
}
