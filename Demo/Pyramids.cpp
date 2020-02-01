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

#define IMG11_PATH "D:\\code\\dan-qt\\Demo\\test1.bmp"
#define IMG12_PATH "D:\\code\\dan-qt\\Demo\\test2.bmp"
#define IMG21_PATH "D:\\code\\dan-qt\\Demo\\left.png"
#define IMG22_PATH "D:\\code\\dan-qt\\Demo\\right.png"
#define AVATAR1_PATH "D:\\code\\dan-qt\\Demo\\avatar.jpg"
#define AVATAR2_PATH "D:\\code\\dan-qt\\Demo\\avatar2.jpg"
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

// 浮点图像
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

// 完美反射  白平衡算法
Mat PerfectReflectionAlgorithm(Mat src) {
    int row = src.rows;
    int col = src.cols;
    Mat dst(row, col, CV_8UC3);
    int HistRGB[767] = { 0 };
    int MaxVal = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            MaxVal = max(MaxVal, (int)src.at<Vec3b>(i, j)[0]);
            MaxVal = max(MaxVal, (int)src.at<Vec3b>(i, j)[1]);
            MaxVal = max(MaxVal, (int)src.at<Vec3b>(i, j)[2]);
            int sum = src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i, j)[2];
            HistRGB[sum]++;
        }
    }
    int Threshold = 0;
    int sum = 0;
    for (int i = 766; i >= 0; i--) {
        sum += HistRGB[i];
        if (sum > row * col * 0.1) {
            Threshold = i;
            break;
        }
    }
    int AvgB = 0;
    int AvgG = 0;
    int AvgR = 0;
    int cnt = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int sumP = src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i, j)[2];
            if (sumP > Threshold) {
                AvgB += src.at<Vec3b>(i, j)[0];
                AvgG += src.at<Vec3b>(i, j)[1];
                AvgR += src.at<Vec3b>(i, j)[2];
                cnt++;
            }
        }
    }
    AvgB /= cnt;
    AvgG /= cnt;
    AvgR /= cnt;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int Blue = src.at<Vec3b>(i, j)[0] * MaxVal / AvgB;
            int Green = src.at<Vec3b>(i, j)[1] * MaxVal / AvgG;
            int Red = src.at<Vec3b>(i, j)[2] * MaxVal / AvgR;
            if (Red > 255) {
                Red = 255;
            }
            else if (Red < 0) {
                Red = 0;
            }
            if (Green > 255) {
                Green = 255;
            }
            else if (Green < 0) {
                Green = 0;
            }
            if (Blue > 255) {
                Blue = 255;
            }
            else if (Blue < 0) {
                Blue = 0;
            }
            dst.at<Vec3b>(i, j)[0] = Blue;
            dst.at<Vec3b>(i, j)[1] = Green;
            dst.at<Vec3b>(i, j)[2] = Red;
        }
    }
    return dst;
}

// 直方图均衡
Mat histogramCal(const Mat& image) {
    int histSize = 255;                //直方图的最大像素值
    float range[] = { 0,256 };
    const float* histRange = { range };
    vector<Mat> bgr;                   //存储图像的矩阵
    split(image, bgr);                 //将彩色图像分割成，b,g,r分别存储
    bool uniform = true, accumulate = false;
    Mat b_hist, g_hist, r_hist;
    //分别计算各个波段的直方图
    calcHist(&bgr[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgr[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgr[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

    //绘制直方图
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8U, Scalar(0, 0, 0));
    //将结果归一化[0,histImage.rows]
    normalize(b_hist,b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))), Scalar(255, 0, 0));
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))), Scalar(0, 255, 0));
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))), Scalar(0, 0, 255));
    }
    return histImage;
}

//  CV_32FC3 转 CV_8UC3
void convertTo8UC3(Mat& imageFrom, Mat& imageTo) {
    int height = imageFrom.rows;
    int width = imageFrom.cols;

    // 方法3
    double min = 0.0;
    double max = 1.0;
    minMaxIdx(imageFrom, &min, &max);

    // 方法1
    // normalize(imageFrom, imageFrom, 0, 1, NORM_L2);

    // 方法2
    // imageFrom *= 128;
    // imageFrom += 200;

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            for (int rgb = 0; rgb < 3; rgb++) {
                double value = (imageFrom.at<Vec3f>(i, j)[rgb] - min) * 255 / (max - min);
                // double value = (imageFrom.at<Vec3f>(i, j)[rgb]);

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

// 图像增强
void ssr(Mat& src, Mat& dst, double sigma) {
    Mat srcFloat = Mat::zeros(src.size(), CV_32FC3);
    Mat srcBlur = Mat::zeros(src.size(), src.type());
    Mat srcBlurFloat = Mat::zeros(src.size(), CV_32FC3);
    Mat result = Mat::zeros(src.size(), CV_32FC3);

    // 源图像转换成浮点
    src.convertTo(srcFloat, CV_32FC3);

    // 转对数
    srcFloat += 0.01;
    log(srcFloat, srcFloat);

    // 源图像滤波
    GaussianBlur(src, srcBlur, Size(0, 0), sigma);
    srcBlur.convertTo(srcBlurFloat, CV_32FC3);
    srcBlurFloat += 0.01;
    // 转对数
    srcFloat += 0.01;
    log(srcBlurFloat, srcBlurFloat);

    result = (srcFloat - srcBlurFloat) / log(10);
    imshow("float", result);
    convertTo8UC3(result, dst);
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
        srcFloat += 0.01;
        log(srcFloat, srcFloat);

        // 源图像滤波
        GaussianBlur(img, srcBlur, Size(0, 0), sigmas[i]);
        srcBlur.convertTo(srcBlurFloat, CV_32FC3);
        srcBlurFloat += 0.01;
        // 转对数
        srcFloat += 0.01;
        log(srcBlurFloat, srcBlurFloat);

        result = (srcFloat - srcBlurFloat) / log(10);

        matrices.emplace_back(result);
    }

    Mat result = Mat::zeros(img.size(), CV_32FC3);
    result = std::accumulate(matrices.begin(), matrices.end(), result) / sigmas.size();
    convertTo8UC3(result, dst);
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

int main() {
    LapPyr LA;
    LapPyr LB;
    LapPyr LS;

    // AVATAR_PATH IMG1_PATH

    // 图像A 拉普拉斯金字塔
    Mat srcA = imread(AVATAR1_PATH);
    Mat srcASSR = Mat::zeros(srcA.size(), CV_8UC3);
    // ssr(srcA, srcASSR, 80);
    msr(srcA, srcASSR, {15, 80, 250});
    imshow("ssr", srcASSR);
    buildLaplacianPyramids(srcASSR, LA);
    // showLaplacianPyramids(LA);

    // 图像B 拉普拉斯金字塔
    Mat srcB = imread(AVATAR2_PATH);
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
