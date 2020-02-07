#include "top.h"
#include "opencv_top.h"
using namespace std;
using namespace cv;
int main (int argc, char** argv)
{
	// 获取图像数据
	IplImage* src = cvLoadImage(INPUT_IMAGE);
	IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);

	// 使用HLS库进行处理
	AXI_STREAM src_axi, dst_axi;
	IplImage2AXIvideo(src, src_axi);
	hls_sobel(src_axi, dst_axi, src->height, src->width);
	AXIvideo2IplImage(dst_axi, dst);
	cvSaveImage(OUTPUT_IMAGE,dst);
	cvShowImage("hls_dst", dst);

	// 使用OPENCV库进行处理
	opencv_image_filter(src, dst);
	cvShowImage("cv_dst", dst);
	cvSaveImage(OUTPUT_IMAGE_GOLDEN,dst);
	waitKey(0);
	// 释放内存
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
}
