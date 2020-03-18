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

#include "gaussian_pyramid.h"
#include "laplacian_pyramid.h"
#include "opencv_utils.h"
#include "remapping_function.h"

#include <iostream>
#include <sstream>

using namespace std;

void OutputBinaryImage(const std::string& filename, cv::Mat image) {
  FILE* f = fopen(filename.c_str(), "wb");
  for (int x = 0; x < image.cols; x++) {
    for (int y = 0; y < image.rows; y++) {
      double tmp = image.at<double>(y, x);
      fwrite(&tmp, sizeof(double), 1, f);
    }
  }
  fclose(f);
}

// Perform Local Laplacian filtering on the given image.
//
// Arguments:
//  input    The input image. Can be any type, but will be converted to double
//           for computation.
//  alpha    Exponent for the detail remapping function. (< 1 for detail
//           enhancement, > 1 for detail suppression)
//  beta     Slope for edge remapping function (< 1 for tone mapping, > 1 for
//           inverse tone mapping)
//  sigma_r  Edge threshold (in image range space).
template<typename T>
LaplacianPyramid LocalLaplacianFilter(const cv::Mat& input,
                             double alpha,
                             double beta,
                             double sigma_r) {
  RemappingFunction r(alpha, beta);

  // int num_levels = LaplacianPyramid::GetLevelCount(input.rows, input.cols, 30);
  int num_levels = 2;
  cout << "Number of levels: " << num_levels << endl;

  const int kRows = input.rows;
  const int kCols = input.cols;

  GaussianPyramid gauss_input(input, num_levels);

  // Construct the unfilled Laplacian pyramid of the output. Copy the residual
  // over from the top of the Gaussian pyramid.
  LaplacianPyramid output(kRows, kCols, input.channels(), num_levels);
  gauss_input[num_levels].copyTo(output[num_levels]);

  // Calculate each level of the ouput Laplacian pyramid.
  for (int l = 0; l < num_levels; l++) {
    int subregion_size = 3 * ((1 << (l + 2)) - 1);
    int subregion_r = subregion_size / 2;

    for (int y = 0; y < output[l].rows; y++) {
      // Calculate the y-bounds of the region in the full-res image.
      int full_res_y = (1 << l) * y;
      int roi_y0 = full_res_y - subregion_r;
      int roi_y1 = full_res_y + subregion_r + 1;
      cv::Range row_range(max(0, roi_y0), min(roi_y1, kRows));
      int full_res_roi_y = full_res_y - row_range.start;

      for (int x = 0; x < output[l].cols; x++) {
        // Calculate the x-bounds of the region in the full-res image.
        int full_res_x = (1 << l) * x;
        int roi_x0 = full_res_x - subregion_r;
        int roi_x1 = full_res_x + subregion_r + 1;
        cv::Range col_range(max(0, roi_x0), min(roi_x1, kCols));
        int full_res_roi_x = full_res_x - col_range.start;

        // Remap the region around the current pixel.
        cv::Mat r0 = input(row_range, col_range);
        cv::Mat remapped;
        r.Evaluate<T>(r0, remapped, gauss_input[l].at<T>(y, x), sigma_r);

        // Construct the Laplacian pyramid for the remapped region and copy the
        // coefficient over to the ouptut Laplacian pyramid.
        LaplacianPyramid tmp_pyr(remapped, l + 1,
            {row_range.start, row_range.end - 1,
             col_range.start, col_range.end - 1});
        output.at<T>(l, y, x) = tmp_pyr.at<T>(l, full_res_roi_y >> l,
                                                 full_res_roi_x >> l);
      }
//      cout << "Level " << (l+1) << " (" << output[l].rows << " x "
//           << output[l].cols << "), footprint: " << subregion_size << "x"
//           << subregion_size << " ... " << round(100.0 * y / output[l].rows)
//           << "%\r";
//      cout.flush();
    }
    stringstream ss;
    ss << "level" << l << ".png";
    cv::imwrite(ss.str(), ByteScale(cv::abs(output[l])));
    cout << endl;
  }

  stringstream ss;
  ss << "level" << num_levels << ".png";
  cv::imwrite(ss.str(), ByteScale(cv::abs(output[num_levels])));
  cout << endl;

  return output;
}

void SalientRegionDetectionBasedonFT(Mat &src, Mat &dst, Mat &sal){
    Mat Lab;
    cvtColor(src, Lab, CV_BGR2Lab);

    int row=src.rows,col=src.cols;

    int Sal_org[row][col];
    memset(Sal_org,0,sizeof(Sal_org));

    Point3_<uchar>* p;

    int MeanL=0,Meana=0,Meanb=0;
    for (int i=0;i<row;i++){
        for (int j=0;j<col;j++){
            p=Lab.ptr<Point3_<uchar> > (i,j);
            MeanL+=p->x;
            Meana+=p->y;
            Meanb+=p->z;
        }
    }
    MeanL/=(row*col);
    Meana/=(row*col);
    Meanb/=(row*col);

    GaussianBlur(Lab,Lab,Size(3,3),0,0);

    int val;

    int max_v=0;
    int min_v=1<<28;

    for (int i=0;i<row;i++){
        for (int j=0;j<col;j++){
            p=Lab.ptr<Point3_<uchar> > (i,j);
            val=sqrt( (MeanL - p->x)*(MeanL - p->x)+ (p->y - Meana)*(p->y-Meana) + (p->z - Meanb)*(p->z - Meanb) );
            Sal_org[i][j]=val;
            max_v=max(max_v,val);
            min_v=min(min_v,val);
        }
    }

    cout<<max_v<<" "<<min_v<<endl;
    int X,Y;
    for (Y = 0; Y < row; Y++)
    {
        for (X = 0; X < col; X++)
        {
            dst.at<float>(Y,X) = (float)(Sal_org[Y][X] - min_v)*1/(max_v - min_v);
            sal.at<uchar>(Y,X) = (Sal_org[Y][X] - min_v)*255/(max_v - min_v);
        }
    }
    // imshow("sal",sal);
    waitKey(0);
}

// 将mat输出到文件，便于调试
void writeMatToFile(Mat& m, const char* filename) {
    ofstream fout(filename);

    if (!fout) {
        std::cout << "File Not Opened" << std::endl;
        return;
    }

    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            fout << m.at<float>(i, j) << "\t";
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
            if (image.channels() == 1) {
                image.at<float>(i, j) = a * image.at<float>(i, j) + b;
            } else {
                for (int rgb = 0; rgb < 3; rgb++) {
                    image.at<Vec3f>(i, j)[rgb] = a * image.at<Vec3f>(i, j)[rgb] + b;
                }
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

// 计算并打印图片的亮度
void showBrightness(Mat& image) {
    Mat gray;
    double brightness = 0.0;
    cvtColor(image, gray, CV_RGB2GRAY);
    Scalar scalar = mean(gray);
    brightness = scalar.val[0];

    std::cout << "brightnes=" << brightness << std::endl;
}

//  将ssr msr处理过的图像从对数空间转到实数空间（线性运算）  x*128 + 128
void convertTo8UC3Way1(Mat& imageFrom, Mat& imageTo) {
    int height = imageFrom.rows;
    int width = imageFrom.cols;

    linearTransform(imageFrom, 128, 128);

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if (imageFrom.channels() == 1) {
                // double value = (imageFrom.at<Vec3f>(i, j)[rgb] - min) * 255 / (max - min);
                double value = (imageFrom.at<float>(i, j));

                if ( value > 255) {
                    imageTo.at<uchar>(i, j) = 255;
                } else if (value < 0) {
                    imageTo.at<uchar>(i, j) = 0;
                } else {
                    imageTo.at<uchar>(i, j) = (uchar)value;
                }
            } else {
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
            if (imageFrom.channels() == 1) {
                double value = (imageFrom.at<float>(i, j) - mins[0]) * 255 / (maxs[0] - mins[0]);

                if ( value > 255) {
                    imageFrom.at<float>(i, j) = 255;
                } else if (value < 0) {
                    imageFrom.at<float>(i, j) = 0;
                } else {
                    imageFrom.at<float>(i, j) = (uchar)value;
                }
            } else {
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
    }

    // 转换成8bit图像
    convertScaleAbs(imageFrom, imageTo);
}

// 重新调整图片亮度
void restoreBrightness(Mat& imageFrom) {
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
            if (imageFrom.channels() == 1) {
                double value = (imageFrom.at<uchar>(i, j) - mins[0]) * 255 / (maxs[0] - mins[0]);

                if ( value > 255) {
                    imageFrom.at<uchar>(i, j) = 255;
                } else if (value < 0) {
                    imageFrom.at<uchar>(i, j) = 0;
                } else {
                    imageFrom.at<uchar>(i, j) = (uchar)value;
                }
            } else {
                for (int rgb = 0; rgb < 3; rgb++) {
                    double value = (imageFrom.at<Vec3b>(i, j)[rgb] - mins[rgb]) * 255 / (maxs[rgb] - mins[rgb]);

                    if ( value > 255) {
                        imageFrom.at<Vec3b>(i, j)[rgb] = 255;
                    } else if (value < 0) {
                        imageFrom.at<Vec3b>(i, j)[rgb] = 0;
                    } else {
                        imageFrom.at<Vec3b>(i, j)[rgb] = (uchar)value;
                    }
                }
            }

        }
    }
}

//   CV_8UC3 转 CV_32FC3
void convertTo32FC3(Mat& imageFrom, Mat& imageTo) {
    int height = imageFrom.rows;
    int width = imageFrom.cols;

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if (imageFrom.channels() == 1) {
                imageTo.at<float>(i, j) = imageFrom.at<uchar>(i, j);
            } else {
                for (int rgb = 0; rgb < 3; rgb++) {
                    imageTo.at<Vec3f>(i, j)[rgb] = imageFrom.at<Vec3b>(i, j)[rgb];
                }
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
            if (image.channels() == 1) {
                image.at<float>(i, j) += THRESHOLD_LOG;
            } else {
                for (int rgb = 0; rgb < 3; rgb++) {
                    image.at<Vec3f>(i, j)[rgb] += THRESHOLD_LOG;
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
void buildLaplacianPyramids(Mat& src, LapPyr& pyr, int octvs=3) {
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
                double deltaA[3] = {0};
                double deltaB[3] = {0};
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
                        if (imageA.channels() == 1) {
                            int x = i + rowOffset;
                            int y = j + colOffset;
                            deltaA[0] += sqrt(0.5 * pow(imageA.at<uchar>(x, y) - imageA.at<uchar>(x - 1, y), 2) + 0.5 * pow(imageA.at<uchar>(x, y) - imageA.at<uchar>(x, y - 1), 2));
                            deltaB[0] += sqrt(0.5 * pow(imageB.at<uchar>(x, y) - imageB.at<uchar>(x - 1, y), 2) + 0.5 * pow(imageB.at<uchar>(x, y) - imageB.at<uchar>(x, y - 1), 2));

                        } else {
                            for (int rgb = 0; rgb < 3; rgb++) {
                                int x = i + rowOffset;
                                int y = j + colOffset;
                                deltaA[rgb] += sqrt(0.5 * pow(imageA.at<Vec3b>(x, y)[rgb] - imageA.at<Vec3b>(x - 1, y)[rgb], 2) + 0.5 * pow(imageA.at<Vec3b>(x, y)[rgb] - imageA.at<Vec3b>(x, y - 1)[rgb], 2));
                                deltaB[rgb] += sqrt(0.5 * pow(imageB.at<Vec3b>(x, y)[rgb] - imageB.at<Vec3b>(x - 1, y)[rgb], 2) + 0.5 * pow(imageB.at<Vec3b>(x, y)[rgb] - imageB.at<Vec3b>(x, y - 1)[rgb], 2));
                            }
                        }

                    }
                }
                if (imageA.channels() == 1) {
                    if (deltaA[0] == deltaB[0]) {
                        imageS.at<uchar>(i, j) = 0.5 * imageA.at<uchar>(i, j) + 0.5 * imageB.at<uchar>(i, j);
                    } else if (deltaA[0] > deltaB[0]) {
                        imageS.at<uchar>(i, j) = imageA.at<uchar>(i, j);
                    } else {
                        imageS.at<uchar>(i, j) = imageB.at<uchar>(i, j);
                    }
                } else {
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
                }


            } else {
                if (imageA.channels() == 1) {
                    imageS.at<uchar>(i, j) = 0.5 * imageA.at<uchar>(i, j) + 0.5 * imageB.at<uchar>(i, j);
                } else {
                    // 边界55开填充
                    for (int rgb = 0; rgb < 3; rgb++) {
                        imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                    }
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
                        if (imageA.channels() == 1) {
                            int x = i + rowOffset;
                            int y = j + colOffset;

                            deltaA[0] += G[1+rowOffset][1+colOffset] * pow(imageA.at<uchar>(x, y), 2);
                            deltaB[0] += G[1+rowOffset][1+colOffset] * pow(imageB.at<uchar>(x, y), 2);
                            matchDegree[0] += G[1+rowOffset][1+colOffset] * imageA.at<uchar>(x, y) * imageB.at<uchar>(x, y);
                        } else {
                            for (int rgb = 0; rgb < 3; rgb++) {
                                int x = i + rowOffset;
                                int y = j + colOffset;

                                deltaA[rgb] += G[1+rowOffset][1+colOffset] * pow(imageA.at<Vec3b>(x, y)[rgb], 2);
                                deltaB[rgb] += G[1+rowOffset][1+colOffset] * pow(imageB.at<Vec3b>(x, y)[rgb], 2);
                                matchDegree[rgb] += G[1+rowOffset][1+colOffset] * imageA.at<Vec3b>(x, y)[rgb] * imageB.at<Vec3b>(x, y)[rgb];
                            }
                        }

                    }
                }
                if (imageA.channels() == 1) {
                    matchDegree[0] = pow(matchDegree[0], 2) / (deltaA[0] * deltaB[0]);

                    if (isnan(matchDegree[0]) || matchDegree[0] < matchDegreeLimit) {
                        if (deltaA[0] == deltaB[0]) {
                            imageS.at<uchar>(i, j) = 0.5 * imageA.at<uchar>(i, j) + 0.5 * imageB.at<uchar>(i, j);
                        } else if (deltaA[0] > deltaB[0]) {
                            imageS.at<uchar>(i, j) = imageA.at<uchar>(i, j);
                        } else {
                            imageS.at<uchar>(i, j) = imageB.at<uchar>(i, j);
                        }
                    } else {
                        double wMin = 0.5 * (1 - (1 - matchDegree[0])/(1 - matchDegreeLimit));
                        imageS.at<uchar>(i, j) = min(imageA.at<uchar>(i, j), imageB.at<uchar>(i, j)) * wMin + max(imageA.at<uchar>(i, j), imageB.at<uchar>(i, j)) * (1 - wMin);
                    }
                } else {
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
                }

            } else {
                if (imageA.channels() == 1) {
                    imageS.at<uchar>(i, j) = 0.5 * imageA.at<uchar>(i, j) + 0.5 * imageB.at<uchar>(i, j);
                } else {
                    // 边界55开填充
                    for (int rgb = 0; rgb < 3; rgb++) {
                        imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                    }
                }

            }
        }
    }
}

//// 融合策略为区域能量
//void blendLaplacianPyramidsByRE1(Mat& imageA, Mat& imageB, Mat& imageS) {
//    int G[3][3] = {
//        {1, 2, 1},
//        {2, 4, 2},
//        {1, 2, 1}
//    };
//    int height = imageA.rows;
//    int width = imageB.cols;

//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {

//            // 不检查边界
//            if ((i > 1) && (i < (height - 2)) && (j > 1) && (j < (width - 2))) {
//                // 3*3
//                int deltaA[3] = {0};
//                int deltaB[3] = {0};
//                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
//                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
//                        for (int rgb = 0; rgb < 3; rgb++) {
//                            int x = i + rowOffset;
//                            int y = j + colOffset;

//                            deltaA[rgb] += G[1+rowOffset][1+colOffset] * imageA.at<Vec3b>(x, y)[rgb];
//                            deltaB[rgb] += G[1+rowOffset][1+colOffset] * imageB.at<Vec3b>(x, y)[rgb];
//                        }
//                    }
//                }
//                for (int rgb = 0; rgb < 3; rgb++) {
//                    if (deltaA[rgb] == deltaB[rgb]) {
//                        imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
//                    } else if (deltaA[rgb] > deltaB[rgb]) {
//                        imageS.at<Vec3b>(i, j)[rgb] = imageA.at<Vec3b>(i, j)[rgb];
//                    } else {
//                        imageS.at<Vec3b>(i, j)[rgb] = imageB.at<Vec3b>(i, j)[rgb];
//                    }
//                }
//            } else {
//                // 边界55开填充
//                for (int rgb = 0; rgb < 3; rgb++) {
//                    imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
//                }
//            }
//        }
//    }
//}

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

// 顶层融合策略: 边缘、显著性增强
void blendLaplacianPyramidsByHVS(Mat& imageA, Mat& imageB, Mat& imageS) {

    Mat imageA_sal = Mat::zeros(imageA.size(),CV_32FC1 );
    Mat imageB_sal = Mat::zeros(imageB.size(),CV_32FC1 );

    Mat imageA_sal_gray = Mat::zeros(imageA.size(),CV_8UC1 );
    Mat imageB_sal_gray = Mat::zeros(imageB.size(),CV_8UC1 );

    SalientRegionDetectionBasedonFT(imageA, imageA_sal, imageA_sal_gray);
    SalientRegionDetectionBasedonFT(imageB, imageB_sal, imageB_sal_gray);

    // 均值滤波
    double G[3][3] = {
        {0.1111, 0.1111, 0.1111},
        {0.1111, 0.1111, 0.1111},
        {0.1111, 0.1111, 0.1111}
    };
    double matchDegreeLimit = 0.1;

    int height = imageA.rows;
    int width = imageB.cols;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            // 不检查边界
            if ((i > 1) && (i < (height - 2)) && (j > 1) && (j < (width - 2))) {
                // 3*3
                double deltaA[3] = {0.0};
                double deltaB[3] = {0.0};
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
                        for (int rgb = 0; rgb < 3; rgb++) {
                            int x = i + rowOffset;
                            int y = j + colOffset;

                            deltaA[rgb] += G[1+rowOffset][1+colOffset] * imageA_sal.at<float>(x, y);
                            deltaB[rgb] += G[1+rowOffset][1+colOffset] * imageB_sal.at<float>(x, y);
                        }
                    }
                }

//                for (int rgb = 0; rgb < 3; rgb++) {
//                    std::cout << 666 << std::endl;
//                    std::cout << fabs(deltaA[rgb] - deltaB[rgb]) << std::endl;
//                    if (fabs(deltaA[rgb] - deltaB[rgb])  > matchDegreeLimit) {
//                        if (deltaA[rgb] == deltaB[rgb]) {
//                            imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
//                        } else if (deltaA[rgb] > deltaB[rgb]) {
//                            imageS.at<Vec3b>(i, j)[rgb] = imageA.at<Vec3b>(i, j)[rgb];
//                        } else {
//                            imageS.at<Vec3b>(i, j)[rgb] = imageB.at<Vec3b>(i, j)[rgb];
//                        }
//                    } else {
//                        double alpha = deltaA[rgb] / deltaA[rgb] + deltaB[rgb];
//                        imageS.at<Vec3b>(i, j)[rgb] = alpha * imageA.at<Vec3b>(i, j)[rgb] + (1 - alpha) * imageB.at<Vec3b>(i, j)[rgb];
//                    }
//                }
                for (int rgb = 0; rgb < 3; rgb++) {
//                    std::cout << 666 << std::endl;
//                    std::cout << fabs(deltaA[rgb] - deltaB[rgb]) << std::endl;
//                    if (fabs(imageA_sal.at<float>(i, j) - imageA_sal.at<float>(i, j))  > matchDegreeLimit) {
//                        if (deltaA[rgb] == deltaB[rgb]) {
//                            imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
//                        } else if (deltaA[rgb] > deltaB[rgb]) {
//                            imageS.at<Vec3b>(i, j)[rgb] = (unsigned char)fmax(imageA.at<Vec3b>(i, j)[rgb], 255 * deltaA[rgb]);
//                        } else {
//                            imageS.at<Vec3b>(i, j)[rgb] = (unsigned char)fmax(imageB.at<Vec3b>(i, j)[rgb], 255 * deltaB[rgb]);
//                        }
//                    } else {
                        double alpha = deltaA[rgb] / deltaA[rgb] + deltaB[rgb];
                        imageS.at<Vec3b>(i, j)[rgb] = alpha * fmax(imageA.at<Vec3b>(i, j)[rgb], 255 * deltaA[rgb]) + (1 - alpha) * fmax(imageB.at<Vec3b>(i, j)[rgb], 255 * deltaB[rgb]);
                    //}
                }
            } else {
                // 边界55开填充
                for (int rgb = 0; rgb < 3; rgb++) {
                    imageS.at<Vec3b>(i, j)[rgb] = 0.5 * imageA.at<Vec3b>(i, j)[rgb] + 0.5 * imageB.at<Vec3b>(i, j)[rgb];
                }
            }
        }
    }

//    Mat grayA;
//    Mat grayB;
//    double brightnessA = 0.0;
//    double brightnessB = 0.0;

//    cvtColor(imageA, grayA, CV_RGB2GRAY);
//    Scalar scalar = mean(grayA);
//    brightnessA = scalar.val[0];

//    cvtColor(imageB, grayB, CV_RGB2GRAY);
//    scalar = mean(grayB);
//    brightnessB = scalar.val[0];

//    std::cout << brightnessA << std::endl;
//    std::cout << brightnessB << std::endl;

//    if (brightnessA >= brightnessB) {
//        imageS = imageA;
//    } else {
//        imageS = imageB;
//    }
}


void SalientRegionDetectionBasedonAC(Mat &src,int MinR2, int MaxR2,int Scale){
    Mat Lab;
    cvtColor(src, Lab, CV_BGR2Lab);

    int row=src.rows,col=src.cols;
    int Sal_org[row][col];
    memset(Sal_org,0,sizeof(Sal_org));

    Mat Sal=Mat::zeros(src.size(),CV_8UC1 );

    Point3_<uchar>* p;
    Point3_<uchar>* p1;
    int val;
    Mat filter;

    int max_v=0;
    int min_v=1<<28;
    for (int k=0;k<Scale;k++){
        int len=(MaxR2 - MinR2) * k / (Scale - 1) + MinR2;
        blur(Lab, filter, Size(len,len ));
        for (int i=0;i<row;i++){
            for (int j=0;j<col;j++){
                p=Lab.ptr<Point3_<uchar> > (i,j);
                p1=filter.ptr<Point3_<uchar> > (i,j);
                //cout<<(p->x - p1->x)*(p->x - p1->x)+ (p->y - p1->y)*(p->y-p1->y) + (p->z - p1->z)*(p->z - p1->z) <<" ";

                val=sqrt( (p->x - p1->x)*(p->x - p1->x)+ (p->y - p1->y)*(p->y-p1->y) + (p->z - p1->z)*(p->z - p1->z) );
                Sal_org[i][j]+=val;
                if(k==Scale-1){
                    max_v=max(max_v,Sal_org[i][j]);
                    min_v=min(min_v,Sal_org[i][j]);
                }
            }
        }
    }

    cout<<max_v<<" "<<min_v<<endl;
    int X,Y;
    for (Y = 0; Y < row; Y++)
    {
        for (X = 0; X < col; X++)
        {
            Sal.at<uchar>(Y,X) = (Sal_org[Y][X] - min_v)*255/(max_v - min_v);        //    计算全图每个像素的显著性
            //Sal.at<uchar>(Y,X) = (Dist[gray[Y][X]])*255/(max_gray);        //    计算全图每个像素的显著性
        }
    }
    imshow("sal",Sal);
    waitKey(0);
}


void showLaplacianPyramids(LapPyr& pyr, char flag) {
    for(int i = 0; i < pyr.size(); i++) {
        stringstream ss;
        ss << flag << "level" << i << ".png";
        cv::imwrite(ss.str(), ByteScale(cv::abs(pyr[i])));
        // cv::imwrite(ss.str(), pyr[i]);
        // waitKey(0);
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
            // blendLaplacianPyramidsByRE1(pyrA[idx], pyrB[idx], pyrS[idx]);
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

        case 5:
//            if (idx == pyrS.size() - 1) {
//                blendLaplacianPyramidsByHVS(pyrA[idx], pyrB[idx], pyrS[idx]);
//            } else {
//                blendLaplacianPyramidsByRE2(pyrA[idx], pyrB[idx], pyrS[idx]);
//            }
            break;
        default:
            break;
        }
    }

    showLaplacianPyramids(pyrS, 'S');

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

LapPyr buildLaplacianPyramidsLLF(Mat& input_rgb) {
  const double kSigmaR = 0.3;
  const double kAlpha = 1;
  const double kBeta = 0;

  cv::Mat input;
  cv::cvtColor(input_rgb, input, CV_RGB2GRAY);

  input.convertTo(input, CV_64F, 1 / 255.0);

  LaplacianPyramid output = LocalLaplacianFilter<double>(input, kAlpha, kBeta, kSigmaR);
  for (int l = 0; l < output.GetLevel(); l++) {
      output[l] *= 255;
      ByteScale(cv::abs(output[l]));
      output[l].convertTo(output[l], CV_8UC1);
  }
  return output.pyramid_;
}

int main() {
    LapPyr LA;
    LapPyr LB;
    LapPyr LS;

    // AVATAR_PATH IMG1_PATH

    // 图像A 拉普拉斯金字塔
    Mat srcA = imread(IMG11_PATH);

    Mat srcASSR = Mat::zeros(srcA.size(), CV_8UC3);
    // ssr(srcA, srcASSR, 15);
    // msr(srcA, srcASSR, {15, 80, 250});
    // imshow("msr", srcASSR);
    LA = buildLaplacianPyramidsLLF(srcA);
    showLaplacianPyramids(LA, 'A');

    // 图像B 拉普拉斯金字塔
    Mat srcB = imread(IMG12_PATH);
    LB = buildLaplacianPyramidsLLF(srcB);
    showLaplacianPyramids(LB, 'B');

//    // 融合
//    Mat dst1;
//    blendLaplacianPyramids(LA, LB, LS, dst1, 1);
//    restoreBrightness(dst1);
//    imshow("1", dst1);

//    Mat dst2;
//    blendLaplacianPyramids(LA, LB, LS, dst2, 2);
//    restoreBrightness(dst2);
//    imshow("2", dst2);

    Mat dst3;
    blendLaplacianPyramids(LA, LB, LS, dst3, 3);
    // restoreBrightness(dst3);
    imshow("3", dst3);

//    Mat dst4;
//    blendLaplacianPyramids(LA, LB, LS, dst4, 4);
//    restoreBrightness(dst4);
//    imshow("4", dst4);

//    Mat dst5;
//    blendLaplacianPyramids(LA, LB, LS, dst5, 5);
//    restoreBrightness(dst5);
//    imshow("5", dst5);

//    showBrightness(dst1);
//    showBrightness(dst2);
//    showBrightness(dst3);
//    showBrightness(dst4);

    waitKey(0);

    // writeMatToFile(dst1, "..\\dst1.txt");
    // writeMatToFile(dst2, "..\\dst2.txt");
    // writeMatToFile(dst3, "..\\dst3.txt");

    return 0;
}


