#ifndef _XF_FUSION_LIB_
#define _XF_FUSION_LIB_

#include "xf_fusion.h"

namespace fusion {

// 平均值标准差包装
template<int ROWS, int COLS>
void meanStdDevWrapper(xf::Mat<_TYPE, ROWS, COLS, _NPC1>& src, double& min, double& max) {
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	unsigned short tmpMean[1];
	unsigned short tmpSd[1];
	xf::meanStdDev<_TYPE, ROWS, COLS, _NPC1>(src, tmpMean, tmpSd);
	double means = (float)tmpMean[0]/256;
	double sds = (float)tmpSd[0]/256;
	min = means - DYNAMIC * sds;
	max = means + DYNAMIC * sds;
}

// 重新调整图片亮度
template<int ROWS, int COLS>
void restoreBrightness(xf::Mat<_TYPE, ROWS, COLS, _NPC1>& src, xf::Mat<_TYPE, ROWS, COLS, _NPC1>& dst, double min, double max) {
#pragma HLS INLINE OFF
	// 归一化
    // 分通道分别计算均值，均方差

    int width = dst.cols;
    int height = dst.rows;
    for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
#pragma HLS UNROLL
        for(int j = 0; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE
            double value = (src.data[i*width+j] - min) * 255 / (max - min);

            if ( value > 255) {
            	dst.write(i*width+j, 255);
            } else if (value < 0) {
            	dst.write(i*width+j, 0);
            } else {
            	dst.write(i*width+j, (unsigned char)value);
            }
        }
    }
}

template<int ROWS, int COLS>
void dstCopyFromSrc(
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& src,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& dst
		) {
#pragma HLS INLINE OFF
	int width = dst.cols;
	int height = dst.rows;
	for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
#pragma HLS UNROLL
		for(int j = 0; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE
			dst.write(i*width+j, src.data[i*width+j]);
		}
	}
}

template<int ROWS, int COLS>
void scaleDown(
		xf::Mat<_TYPE, ROWS*2, COLS*2, _NPC1>& src,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& dst
		) {
#pragma HLS INLINE OFF
	int width = dst.cols;
	int height = dst.rows;
	for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
#pragma HLS UNROLL
		for(int j = 0; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE
			dst.write(i*width+j, src.data[i*width+j]);
		}
	}
}

// 通过源图像构造拉普拉斯金字塔
template<int ROWS, int COLS>
void lapPyrUpSubLevel(
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& pyrX,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& pyrU,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& tempScale1,
		xf::Mat<_TYPE, ROWS*2, COLS*2, _NPC1>& tempScale2,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& dstX
) {
	// 上一层高斯金字塔减去本层高斯金字塔*2得到上一层拉普拉斯金字塔；pyr[0~N]构成了拉普拉斯金字塔；第N层（最后一层）拉普拉斯金字塔同高斯金字塔
	xf::pyrUp<_TYPE, ROWS, COLS,  _NPC1>(pyrX, tempScale2);
	tempScale1.rows = pyrU.rows;
	tempScale1.cols = pyrU.cols;
	fusion::scaleDown(tempScale2, tempScale1);
	{
	#pragma HLS latency min=1 max=1
		xf::absdiff<_TYPE, ROWS, COLS, _NPC1>(pyrU, tempScale1, dstX);
	}
}

// 融合策略为区域能量
template<int ROWS, int COLS>
void blendLaplacianPyramidsByRE2(xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageA, xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageB, xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageS) {
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	// 均值滤波
    static double G[3][3] = {
        {0.1111, 0.1111, 0.1111},
        {0.1111, 0.1111, 0.1111},
        {0.1111, 0.1111, 0.1111}
    };
    double matchDegreeLimit = 0.618;

    int height = imageA.rows;
    int width = imageB.cols;

    for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
        for (int j = 0; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE

            // 不检查边界
            if ((i > 1) && (i < (height - 2)) && (j > 1) && (j < (width - 2))) {
                // 3*3
                static double deltaA = 0.0;
                static double deltaB = 0.0;
                static double matchDegree = 0.0;

                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=3
#pragma HLS UNROLL
                    for (int colOffset= -1; colOffset <= 1; colOffset++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=3
                        // 单通道
                        int x = i + rowOffset;
                        int y = j + colOffset;

                        deltaA += G[1+rowOffset][1+colOffset] * imageA.data[x*width+y] * imageA.data[x*width+y];
                        deltaB += G[1+rowOffset][1+colOffset] * imageB.data[x*width+y] * imageB.data[x*width+y];
                        matchDegree += G[1+rowOffset][1+colOffset] * imageA.data[x*width+y] * imageB.data[x*width+y];
                    }
                }
                // 计算匹配度
                matchDegree = matchDegree * matchDegree / (deltaA * deltaB);

                if (hls::isnan(matchDegree) || matchDegree < matchDegreeLimit) {
                    if (deltaA == deltaB) {
                        imageS.write(i*width+j, 0.5 * imageA.data[i*width+j] + 0.5 * imageB.data[i*width+j]);
                    } else if (deltaA > deltaB) {
                        imageS.write(i*width+j, imageA.data[i*width+j]);
                    } else {
                        imageS.write(i*width+j, imageB.data[i*width+j]);
                    }
                } else {
                    double wMin = 0.5 * (1 - (1 - matchDegree)/(1 - matchDegreeLimit));
                    imageS.write(i*width+j, hls::min(imageA.data[i*width+j], imageB.data[i*width+j]) * wMin + hls::max(imageA.data[i*width+j], imageB.data[i*width+j]) * (1 - wMin));
                }
            } else {
                // 边界55开填充
                imageS.write(i*width+j, 0.5 * imageA.data[i*width+j] + 0.5 * imageB.data[i*width+j]);
            }
        }
    }
}

// 通过源图像构造拉普拉斯金字塔
template<int ROWS, int COLS>
void lapPyrUpAddLevel(
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& pyrX,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& pyrU,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& tempScale1,
		xf::Mat<_TYPE, ROWS*2, COLS*2, _NPC1>& tempScale2,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& dstX
) {
	// 上一层高斯金字塔减去本层高斯金字塔*2得到上一层拉普拉斯金字塔；pyr[0~N]构成了拉普拉斯金字塔；第N层（最后一层）拉普拉斯金字塔同高斯金字塔
	xf::pyrUp<_TYPE, ROWS, COLS,  _NPC1>(pyrX, tempScale2);
	tempScale1.rows = pyrU.rows;
	tempScale1.cols = pyrU.cols;
	fusion::scaleDown(tempScale2, tempScale1);
    xf::add<XF_CONVERT_POLICY_SATURATE, _TYPE, ROWS, COLS, _NPC1>(pyrU, tempScale1, dstX);
}
}
#endif
