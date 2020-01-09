/* Capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *    This program were got from V4L2 API, Draft 0.20
 *        available at: http://v4l2spec.bytesex.org/
 */
#include<opencv2/opencv.hpp>
#include<iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#define FORCED_WIDTH  640
#define FORCED_HEIGHT 480
#define FORCED_FORMAT V4L2_PIX_FMT_YUYV
#define FORCED_FIELD  V4L2_FIELD_ANY

static int verbose = 0;
#define pr_debug(fmt, arg...) \
    if (verbose) fprintf(stderr, fmt, ##arg)

#define CLEAR(x) memset(&(x), 0, sizeof(x))

static unsigned char *buffer_capture_ptr;
static unsigned int buffer_capture_length;
static int frame_pix_width, frame_pix_height;
static int fd = -1;
static int cameraOpenedFlag = 0;

/* ======================================================================== */
/* convert from 4:2:2 YUYV interlaced to RGB24                              */
/* based on ccvt_yuyv_bgr32() from camstream                                */
/* ======================================================================== */
#define SAT(c) if (c & (~255)) { if (c < 0) c = 0; else c = 255; }
static void yuyv_to_rgb24_normal (int width, int height, unsigned char *src, unsigned char *dst)
{
   unsigned char *s;
   unsigned char *d;
   int l, c;
   int r, g, b, cr, cg, cb, y1, y2;

   l = height;
   s = src;
   d = dst;
   while (l--) {
      c = width >> 1;
      while (c--) {
         y1 = *s++;
         cb = ((*s - 128) * 454) >> 8;
         cg = (*s++ - 128) * 88;
         y2 = *s++;
         cr = ((*s - 128) * 359) >> 8;
         cg = (cg + (*s++ - 128) * 183) >> 8;

         r = y1 + cr;
         b = y1 + cb;
         g = y1 - cg;
         SAT(r);
         SAT(g);
         SAT(b);
         *d++ = b;
         *d++ = g;
         *d++ = r;
         r = y2 + cr;
         b = y2 + cb;
         g = y2 - cg;
         SAT(r);
         SAT(g);
         SAT(b);

         *d++ = b;
         *d++ = g;
         *d++ = r;
      }
   }
}

/* ======================================================================== */
/* convert YUV to RGB with flip operation                                   */
/* ======================================================================== */
static void yuyv_to_rgb24_mirrored (int width, int height, unsigned char *src, unsigned char *dst)
{
   unsigned char *s;
   unsigned char *d;
   int l, c;
   int r, g, b, cr, cg, cb, y1, y2;

   l = height;
   s = src;
   d = dst + 3 * width - 1;
   while (l--) {
      c = width >> 1;
      while (c--) {
         y1 = *s++;
         cb = ((*s - 128) * 454) >> 8;
         cg = (*s++ - 128) * 88;
         y2 = *s++;
         cr = ((*s - 128) * 359) >> 8;
         cg = (cg + (*s++ - 128) * 183) >> 8;

         r = y1 + cr;
         b = y1 + cb;
         g = y1 - cg;
         SAT(r);
         SAT(g);
         SAT(b);
         *d-- = r;
         *d-- = g;
         *d-- = b;
         r = y2 + cr;
         b = y2 + cb;
         g = y2 - cg;
         SAT(r);
         SAT(g);
         SAT(b);

         *d-- = r;
         *d-- = g;
         *d-- = b;
      }
      d += 6 * width;
   }
}

/* ======================================================================== */
/* print error number and exit                                              */
/* ======================================================================== */
static void errno_exit(const char *s)
{
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

/* ======================================================================== */
/* make ioctl call                                                          */
/* ======================================================================== */
static int xioctl(int fh, int request, void *arg)
{
    int r;

    do {
      r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

/* ======================================================================== */
/* Read frame and do flipping as needed                                     */
/* ======================================================================== */
static int read_frame(unsigned char *ocv_ptr, int mirrored)
{
        struct timespec tstart, tend;

    if (-1 == read(fd, buffer_capture_ptr, buffer_capture_length)) {
         switch (errno) {
        case EAGAIN:
            return 0;
        case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */
        default:
            errno_exit("read");
       }
    }
    pr_debug("%s: called!\n", __func__);
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        if(mirrored) {
          yuyv_to_rgb24_mirrored (frame_pix_width, frame_pix_height, (unsigned char *)buffer_capture_ptr, ocv_ptr);
        } else {
          yuyv_to_rgb24_normal (frame_pix_width, frame_pix_height, (unsigned char *)buffer_capture_ptr, ocv_ptr);
        }
        clock_gettime(CLOCK_MONOTONIC, &tend);
        if(verbose) pr_debug ("deltaT=%lf ", (tend.tv_sec - tstart.tv_sec) * 1000000.0 + (tend.tv_nsec - tstart.tv_nsec) * 0.001);
    return 1;
}

/* ======================================================================== */
/* top level function to do video capture                                   */
/* ======================================================================== */
void cameraGrabFrame (cv::Mat &ocvFrame, int mirrored)
{
    for (;;) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select(fd + 1, &fds, NULL, NULL, &tv);

        if (-1 == r) {
            if (EINTR == errno)
                continue;
            errno_exit("select");
        }

        if (0 == r) {
            fprintf(stderr, "select timeout\n");
            exit(EXIT_FAILURE);
        }

        if (read_frame(ocvFrame.ptr(0), mirrored)) break;
    /* EAGAIN - continue select loop. */
    }
    fflush(stderr);
}

/* ======================================================================== */
/* Initialize device                                                        */
/* ======================================================================== */
static int init_device(char *dev_name)
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    pr_debug("%s: called!\n", __func__);

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
                        return 0;
        } else {
            fprintf (stderr, "VIDIOC_QUERYCAP");
                        return 0;
        }
    }

    pr_debug("\tdriver: %s\n"
         "\tcard: %s \n"
         "\tbus_info: %s\n",
            cap.driver, cap.card, cap.bus_info);
    pr_debug("\tversion: %u.%u.%u\n",
            (cap.version >> 16) & 0xFF,
            (cap.version >> 8) & 0xFF,
            cap.version & 0xFF);
    pr_debug("\tcapabilities: 0x%08x\n", cap.capabilities);

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n", dev_name);
                return 0;
    }

    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
          fprintf(stderr, "%s does not support read i/o\n", dev_name);
                return 0;
    }

    /* Select video input, video standard and tune here. */
    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
        pr_debug("\tcropcap.type: %d\n", cropcap.type);
        pr_debug("\tcropcap.bounds.left: %d\n", cropcap.bounds.left);
        pr_debug("\tcropcap.bounds.top: %d\n", cropcap.bounds.top);
        pr_debug("\tcropcap.bounds.width: %d\n", cropcap.bounds.width);
        pr_debug("\tcropcap.bounds.height: %d\n", cropcap.bounds.height);

        pr_debug("\tcropcap.defrect.left: %d\n", cropcap.defrect.left);
        pr_debug("\tcropcap.defrect.top: %d\n", cropcap.defrect.top);
        pr_debug("\tcropcap.defrect.width: %d\n", cropcap.defrect.width);
        pr_debug("\tcropcap.defrect.height: %d\n", cropcap.defrect.height);

        pr_debug("\tcropcap.pixelaspect.numerator: %d\n", cropcap.pixelaspect.numerator);
        pr_debug("\tcropcap.pixelaspect.denominator: %d\n", cropcap.pixelaspect.denominator);
        pr_debug("\n");

        CLEAR(crop);
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
            switch (errno) {
            case EINVAL:
                /* Cropping not supported. */
                break;
            default:
                /* Errors ignored. */
                pr_debug("\tcropping not supported\n");
                break;
            }
        }
    } else {
        /* Errors ignored. */
    }
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = FORCED_WIDTH;
        fmt.fmt.pix.height      = FORCED_HEIGHT;
        fmt.fmt.pix.pixelformat = FORCED_FORMAT;
        fmt.fmt.pix.field       = FORCED_FIELD;

        pr_debug("\tfmt.fmt.pix.pixelformat: %c,%c,%c,%c\n",
                fmt.fmt.pix.pixelformat & 0xFF,
                (fmt.fmt.pix.pixelformat >> 8) & 0xFF,
                (fmt.fmt.pix.pixelformat >> 16) & 0xFF,
                (fmt.fmt.pix.pixelformat >> 24) & 0xFF
                );
        pr_debug("\n");

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) return 0;

        frame_pix_width  = fmt.fmt.pix.width;
        frame_pix_height = fmt.fmt.pix.height;
    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;
        pr_debug("\tBytes per line %d frame height %d frame size %d\n",
                 fmt.fmt.pix.bytesperline, fmt.fmt.pix.height, fmt.fmt.pix.sizeimage);
    buffer_capture_length = fmt.fmt.pix.sizeimage;
        buffer_capture_ptr    = (unsigned char *)malloc(buffer_capture_length);
        return 1;
}

/* ======================================================================== */
/* Close device                                                             */
/* ======================================================================== */
int cameraCloseDevice(void)
{
int ret_val = 1;
        if(cameraOpenedFlag) {
        pr_debug("%s: called!\n", __func__);
          cameraOpenedFlag = 0;
      if (-1 == close(fd)) ret_val = 0;
      fd = -1;
          free(buffer_capture_ptr);
        } ret_val = 0;

        return ret_val;
}

/* ======================================================================== */
/* Open device                                                             */
/* ======================================================================== */
int cameraOpenDevice(int devNum)
{
        char dev_name[80];
    struct stat st;

        sprintf (dev_name, "/dev/video%d", devNum);

    pr_debug("%s: called!\n", __func__);

    if (-1 == stat(dev_name, &st)) {
         fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
      return 0;
    }

    if (!S_ISCHR(st.st_mode)) {
      fprintf(stderr, "%s is no device\n", dev_name);
      return 0;
    }
    fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
             dev_name, errno, strerror(errno));
      return 0;
    }
    cameraOpenedFlag = init_device(dev_name);
        return cameraOpenedFlag;
}

cv::Mat cameraCreateMat (void)
{
  return cv::Mat(frame_pix_height, frame_pix_width, CV_8UC3);
}

int cameraIsOpened(void) {
  return cameraOpenedFlag;
}

int cameraGrabWidth(void) {
   return frame_pix_width;
}

int cameraGrabHeight(void) {
   return frame_pix_height;
}

/************************************************************/
