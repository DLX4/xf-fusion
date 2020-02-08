#include "xf_fusion.h"

#define DYNAMIC 2
// 重新调整图片亮度
void restoreBrightness(xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& src, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& dst) {
    // 归一化
    // 分通道分别计算均值，均方差
    double means = 0.0;
    double sds = 0.0;
    double mins = 0.0;
    double maxs = 0.0;

    unsigned short tmpMean[1];
    unsigned short tmpSd[1];
    xf::meanStdDev(src, tmpMean, tmpSd);
    means = tmpMean[0];
    sds = tmpSd[0];
    mins = means - DYNAMIC * sds;
    maxs = means + DYNAMIC * sds;

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
template<int ROWS, int COLS>
void buildLaplacianPyramids(xf::Mat<TYPE, ROWS, COLS, NPC1>& src, xf::Mat<TYPE, ROWS, COLS, NPC1>& pyr0, xf::Mat<TYPE, ROWS/2, COLS/2, NPC1>& pyr1, xf::Mat<TYPE, ROWS/4, COLS/4, NPC1>& pyr2, xf::Mat<TYPE, ROWS/8, COLS/8, NPC1>& pyr3, xf::Mat<TYPE, ROWS/16, COLS/16, NPC1>& pyr4) {
    pyr0 = src;

    // 往下构造本层高斯金字塔 第1层
    xf::pyrDown(pyr0, pyr1);
    // 往下构造本层高斯金字塔 第2层
    xf::pyrDown(pyr1, pyr2);
    // 往下构造本层高斯金字塔 第3层
    xf::pyrDown(pyr2, pyr3);
    // 往下构造本层高斯金字塔 第4层
    xf::pyrDown(pyr3, pyr4);

    // 上一层高斯金字塔减去本层高斯金字塔*2得到上一层拉普拉斯金字塔；pyr[0~N]构成了拉普拉斯金字塔；第N层（最后一层）拉普拉斯金字塔同高斯金字塔
    // 第1层
    xf::Mat<TYPE, ROWS, COLS, NPC1> expend1(pyr0.cols, pyr0.rows);// upscale 2x
    xf::pyrUp(pyr1, expend1);
    xf::addWeighted(pyr0, 1, expend1, -1, 0, pyr0);

    // 第2层
    xf::Mat<TYPE, ROWS/2, COLS/2, NPC1> expend2(pyr1.cols, pyr1.rows);// upscale 2x
    xf::pyrUp(pyr2, expend2);
    xf::addWeighted(pyr1, 1, expend2, -1, 0, pyr1);

    // 第3层
    xf::Mat<TYPE, ROWS/4, COLS/4, NPC1> expend3(pyr2.cols, pyr2.rows);// upscale 2x
    xf::pyrUp(pyr3, expend3);
    xf::addWeighted(pyr2, 1, expend3, -1, 0, pyr2);

    // 第4层
    xf::Mat<TYPE, ROWS/8, COLS/8, NPC1> expend4(pyr3.cols, pyr3.rows);// upscale 2x
    xf::pyrUp(pyr4, expend4);
    xf::addWeighted(pyr3, 1, expend4, -1, 0, pyr3);
}


// 融合策略为区域能量
template<int ROWS, int COLS>
void blendLaplacianPyramidsByRE2(xf::Mat<TYPE, ROWS, COLS, NPC1>& imageA, xf::Mat<TYPE, ROWS, COLS, NPC1>& imageB, xf::Mat<TYPE, ROWS, COLS, NPC1>& imageS) {
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
void blendLaplacianPyramids(xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& pyrA0, xf::Mat<TYPE, HEIGHT/2, WIDTH/2, NPC1>& pyrA1, xf::Mat<TYPE, HEIGHT/4, WIDTH/4, NPC1>& pyrA2, xf::Mat<TYPE, HEIGHT/8, WIDTH/8, NPC1>& pyrA3, xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1>& pyrA4,
                            xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& pyrB0, xf::Mat<TYPE, HEIGHT/2, WIDTH/2, NPC1>& pyrB1, xf::Mat<TYPE, HEIGHT/4, WIDTH/4, NPC1>& pyrB2, xf::Mat<TYPE, HEIGHT/8, WIDTH/8, NPC1>& pyrB3, xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1>& pyrB4,
                            xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& pyrS0, xf::Mat<TYPE, HEIGHT/2, WIDTH/2, NPC1>& pyrS1, xf::Mat<TYPE, HEIGHT/4, WIDTH/4, NPC1>& pyrS2, xf::Mat<TYPE, HEIGHT/8, WIDTH/8, NPC1>& pyrS3, xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1>& pyrS4,
                            xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& dst) {

    // 拉普拉斯金字塔各层分别融合 0 1 2 3 4
    blendLaplacianPyramidsByRE2(pyrA0, pyrB0, pyrS0);
    blendLaplacianPyramidsByRE2(pyrA1, pyrB1, pyrS1);
    blendLaplacianPyramidsByRE2(pyrA2, pyrB2, pyrS2);
    blendLaplacianPyramidsByRE2(pyrA3, pyrB3, pyrS3);
    blendLaplacianPyramidsByRE2(pyrA4, pyrB4, pyrS4);

    // 输出图像 4 3 2 1
    xf::Mat<TYPE, HEIGHT/8, WIDTH/8, NPC1> expend4(pyrS3.cols, pyrS3.rows);// upscale 2x
    xf::pyrUp(pyrS4, expend4);
    xf::addWeighted(pyrS3, 1, expend4, 1, 0, pyrS3);

    xf::Mat<TYPE, HEIGHT/4, WIDTH/4, NPC1> expend3(pyrS2.cols, pyrS2.rows);// upscale 2x
    xf::pyrUp(pyrS3, expend3);
    xf::addWeighted(pyrS2, 1, expend3, 1, 0, pyrS2);

    xf::Mat<TYPE, HEIGHT/2, WIDTH/2, NPC1> expend2(pyrS1.cols, pyrS1.rows);// upscale 2x
    pyrUp(pyrS2, expend2);
    addWeighted(pyrS1, 1, expend2, 1, 0, pyrS1);

    xf::Mat<TYPE, HEIGHT, WIDTH, NPC1> expend1(pyrS0.cols, pyrS0.rows);// upscale 2x
    pyrUp(pyrS1, expend1);
    addWeighted(pyrS0, 1, expend1, 1, 0, pyrS0);

    // 调整亮度
    restoreBrightness(pyrS0, dst);
}

void blend(xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcA, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcB, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& dst) {
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

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrA0(width0, height0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrA1(width1, height1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrA2(width2, height2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrA3(width3, height3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrA4(width4, height4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrB0(width0, height0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrB1(width1, height1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrB2(width2, height2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrB3(width3, height3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrB4(width4, height4);

	xf::Mat<TYPE, HEIGHT,    WIDTH,    NPC1> pyrS0(width0, height0);
	xf::Mat<TYPE, HEIGHT/2,  WIDTH/2,  NPC1> pyrS1(width1, height1);
	xf::Mat<TYPE, HEIGHT/4,  WIDTH/4,  NPC1> pyrS2(width2, height2);
	xf::Mat<TYPE, HEIGHT/8,  WIDTH/8,  NPC1> pyrS3(width3, height3);
	xf::Mat<TYPE, HEIGHT/16, WIDTH/16, NPC1> pyrS4(width4, height4);

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
