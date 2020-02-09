#ifndef _XF_FUSION_H_
#define _XF_FUSION_H_

#include "hls_stream.h"
#include "ap_int.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"
#include "imgproc/xf_dilation.hpp"
#include "imgproc/xf_pyr_down.hpp"
#include "imgproc/xf_pyr_up.hpp"
#include "imgproc/xf_add_weighted.hpp"
#include "core/xf_mean_stddev.hpp"
#include "core/xf_arithm.hpp"
// 数学
#include "hls_math.h"

/* Optimization type */
#define RO  0 // Resource Optimized (8-pixel implementation)
#define NO  1 // Normal Operation (1-pixel implementation)

/* config width and height */
#define WIDTH 	960
#define HEIGHT	960

/*  define the input and output types  */
#if NO
#define NPC1 XF_NPPC1
#endif

#if RO
#define NPC1 XF_NPPC8
#endif

#define TYPE XF_8UC1

// 图像融合顶层函数
void blend(xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcA, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcB, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& dst);

// 构造拉普拉斯金字塔
template<int ROWS, int COLS>
void buildLaplacianPyramids(xf::Mat<TYPE, ROWS, COLS, NPC1>& src, xf::Mat<TYPE, ROWS, COLS, NPC1>& pyr0, xf::Mat<TYPE, ROWS/2, COLS/2, NPC1>& pyr1, xf::Mat<TYPE, ROWS/4, COLS/4, NPC1>& pyr2, xf::Mat<TYPE, ROWS/8, COLS/8, NPC1>& pyr3, xf::Mat<TYPE, ROWS/16, COLS/16, NPC1>& pyr4);

// 将两个原图像的拉普拉斯金字塔融合
template<int ROWS, int COLS>
void blendLaplacianPyramids(xf::Mat<TYPE, ROWS, COLS, NPC1>& pyrA0, xf::Mat<TYPE, ROWS/2, COLS/2, NPC1>& pyrA1, xf::Mat<TYPE, ROWS/4, COLS/4, NPC1>& pyrA2, xf::Mat<TYPE, ROWS/8, COLS/8, NPC1>& pyrA3, xf::Mat<TYPE, ROWS/16, COLS/16, NPC1>& pyrA4,
                            xf::Mat<TYPE, ROWS, COLS, NPC1>& pyrB0, xf::Mat<TYPE, ROWS/2, COLS/2, NPC1>& pyrB1, xf::Mat<TYPE, ROWS/4, COLS/4, NPC1>& pyrB2, xf::Mat<TYPE, ROWS/8, COLS/8, NPC1>& pyrB3, xf::Mat<TYPE, ROWS/16, COLS/16, NPC1>& pyrB4,
                            xf::Mat<TYPE, ROWS, COLS, NPC1>& pyrS0, xf::Mat<TYPE, ROWS/2, COLS/2, NPC1>& pyrS1, xf::Mat<TYPE, ROWS/4, COLS/4, NPC1>& pyrS2, xf::Mat<TYPE, ROWS/8, COLS/8, NPC1>& pyrS3, xf::Mat<TYPE, ROWS/16, COLS/16, NPC1>& pyrS4,
                            xf::Mat<TYPE, ROWS, COLS, NPC1>& dst);

template<int ROWS, int COLS>
void blendLaplacianPyramidsByRE2(xf::Mat<TYPE, ROWS, COLS, NPC1>& imageA, xf::Mat<TYPE, ROWS, COLS, NPC1>& imageB, xf::Mat<TYPE, ROWS, COLS, NPC1>& imageS);

template<int ROWS, int COLS>
void restoreBrightness(xf::Mat<TYPE, ROWS, COLS, NPC1>& src, xf::Mat<TYPE, ROWS, COLS, NPC1>& dst);
#endif
