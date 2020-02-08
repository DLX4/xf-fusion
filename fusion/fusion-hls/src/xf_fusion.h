#ifndef _XF_FUSION_H_
#define _XF_FUSION_H_

#include "hls_stream.h"
#include "ap_int.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"
#include "imgproc/xf_dilation.hpp"
#include "imgproc/xf_pyr_down.hpp"
#include "imgproc/xf_pyr_up.hpp"
#include "imgproc/xf_histogram.hpp"

/* Optimization type */
#define RO  0 // Resource Optimized (8-pixel implementation)
#define NO  1 // Normal Operation (1-pixel implementation)

/* config width and height */
#define WIDTH 	1920
#define HEIGHT	1080

/*  define the input and output types  */
#if NO
#define NPC1 XF_NPPC1
#endif

#if RO
#define NPC1 XF_NPPC8
#endif

#define TYPE XF_8UC3

// Í¼ÏñÈÚºÏ¶¥²ãº¯Êý
void blend(xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcA, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& srcB, xf::Mat<TYPE, HEIGHT, WIDTH, NPC1>& dst);
#endif
