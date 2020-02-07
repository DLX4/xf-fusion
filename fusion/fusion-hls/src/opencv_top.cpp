#include "opencv_top.h"
#include "top.h"
void opencv_image_filter(IplImage* src, IplImage* dst)
{
	IplImage* tmp = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvCopy(src, tmp);
	cv::Sobel((cv::Mat)tmp, (cv::Mat)dst, -1, 1, 0);
	cvSubS(dst, cvScalar(50,50,50), tmp);
	cvScale(tmp, dst, 2, 0);
	cvErode(dst, tmp);
	cvDilate(tmp, dst);
	cvReleaseImage(&tmp);
}
void sw_image_filter(IplImage* src, IplImage* dst)
{
	AXI_STREAM src_axi, dst_axi;
	IplImage2AXIvideo(src, src_axi);
	hls_sobel(src_axi, dst_axi, src->height, src->width);
	AXIvideo2IplImage(dst_axi, dst);
}
