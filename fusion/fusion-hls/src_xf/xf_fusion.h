#ifndef _MY_FUSION_H_
#define _MY_FUSION_H_

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

/* Optimization _TYPE */
#define RO  0 // Resource Optimized (8-pixel implementation)
#define NO  1 // Normal Operation (1-pixel implementation)

/* config width and height */
#define WIDTH 	640
#define HEIGHT	480
#define SIZE 640*480

/*  define the input and output __TYPEs  */
#define _NPC1 XF_NPPC1
#define _TYPE XF_8UC1
//
#define DYNAMIC 2

// 图像融合顶层函数
// void blend(xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& srcA, xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& srcB, xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& dst);
void blend(
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrA1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrB1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS0,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& pyrS1,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& dst,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& tempScale1A,
		xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1>& tempScale2A,
		xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& tempScale1B,
		xf::Mat<_TYPE, HEIGHT*2, WIDTH*2, _NPC1>& tempScale2B
		);
#endif
