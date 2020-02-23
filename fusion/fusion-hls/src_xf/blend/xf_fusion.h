#ifndef _MY_FUSION_H_
#define _MY_FUSION_H_
#include "hls_stream.h"
#include "ap_int.h"
#include "hls_math.h"

#if !defined (__SYNTHESIS__)
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#endif

#include "common/xf_common.h"
#include "common/xf_utility.h"
#if !defined (__SYNTHESIS__)
#include "common/xf_axi.h"
#endif
#include "common/xf_infra.h"
#include "imgproc/xf_dilation.hpp"
#include "imgproc/xf_pyr_down.hpp"
#include "imgproc/xf_pyr_up.hpp"
#include "imgproc/xf_add_weighted.hpp"
#include "core/xf_mean_stddev.hpp"
#include "core/xf_arithm.hpp"


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

// Í¼ÏñÈÚºÏ¶¥²ãº¯Êý
// void blend(xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& srcA, xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& srcB, xf::Mat<_TYPE, HEIGHT, WIDTH, _NPC1>& dst);
void blendTop(
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLA0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLA1,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLB0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrLB1,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrS0,
		hls::stream<ap_axiu<8,1,1,1>>& _pyrS1
		);
#endif
