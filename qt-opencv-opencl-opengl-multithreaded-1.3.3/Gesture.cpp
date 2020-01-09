/*
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
#include<iostream>
#include<sstream>
#include<vector>
#include<algorithm>
#include<cstdio>
#include<unistd.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/ocl.hpp>  
#include "Gesture.h"

//This define will enable contrast adaptation
//#define USE_CONTRAST_ADAPTATION

//This define will enable use of H plan from HSV color space
//#define USE_HPLANE

using namespace std;
using namespace cv;
//Use OpenCL acceleration or A15 execution
bool ocl_bgfg_mog2_flag = true;  //opencl for background MOG2 kernel
bool ocl_morph_flag     = true; //opencl for erode and dilate kernels (or not?)
/* ======================================================================== */
/* Hand gesture recognition                                                 */
/* Calculate the ratio between inner contour area and convex hull area      */
/* (contour connecting tip of fingers)                                      */
/* ======================================================================== */
int Gesture(Mat& srcIN, Mat& dst, int& backgroundFrame, int& needContrast, int* finger_window, cv::Ptr<BackgroundSubtractorMOG2>& bg)
{
        float contAreaMin;
        UMat frame, src;
        UMat frameLC;
        UMat fore, tmp_fore;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        Mat  frameHSV;
        vector<Mat> hsv_planes;
        float cont2hullRatio = 1.0f;
        int retVal = 0;

        srcIN.copyTo(src);
#ifdef USE_CONTRAST_ADAPTATION
        if(backgroundFrame > (BACKGROUND_FRAMES - 5) )
        {
           if(backgroundFrame == BACKGROUND_FRAMES) needContrast = 0;
           cvtColor(srcIN, frameHSV, CV_BGR2HSV);
           split( frameHSV, hsv_planes );
           backgroundFrame --;
           Scalar mean, std_dev;
           meanStdDev(hsv_planes[2], mean, std_dev);
           cout << "Img mean " << mean.val[0] << " Img std_dev " << std_dev.val[0] << endl;
           if((mean.val[0] + std_dev.val[0]) < 30.0) needContrast ++;
           else needContrast --;
           if(backgroundFrame == (BACKGROUND_FRAMES - 5))
           {
              if(needContrast > 2) needContrast = 1;
              else needContrast = 0;
           }
           dst = srcIN;
           return 0;
        }

        if(needContrast) {
          src.convertTo(frame, -1, 2, 0);
        } else frame = src;

#else

#ifdef USE_HPLANE
        cvtColor(src, frameHSV, CV_BGR2HSV);
        split( frameHSV, hsv_planes );
        frame = hsv_planes[0];
#else
        frame = src;
#endif

#endif

        //Update the current background model and get the foreground
      ocl::setUseOpenCL(ocl_bgfg_mog2_flag);
        bg->apply(frame, fore, backgroundFrame > 0 ? -1 : 0);
        if(backgroundFrame>0)
        {
           backgroundFrame--;
        }

        //Enhance edges in the foreground by applying erosion and dilation
      ocl::setUseOpenCL(ocl_morph_flag);
        erode(fore,tmp_fore,UMat());
        dilate(tmp_fore,fore,UMat(), Point(-1,-1));
      ocl::setUseOpenCL(false);
        for(int i = 0; i < (FINGER_WINDOW_SIZE - 1); i ++) finger_window[i] = finger_window[i+1];
        finger_window[FINGER_WINDOW_SIZE - 1] = -1;

        //Find the contours in the foreground
        findContours(fore, contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        contAreaMin = (frame.rows * frame.cols) / 20;
        for(unsigned int i=0; i < contours.size(); i++)
        {
                float currContArea = contourArea(contours[i]);
                //Ignore all small insignificant areas
                if(contourArea(contours[i])>=contAreaMin)
                {
                      //Draw contour
                      vector<vector<Point> > tcontours;
                      tcontours.push_back(contours[i]);
                      drawContours(src,tcontours,-1,cv::Scalar(0,0,255),2);
                      //Detect Hull in current contour
                      vector<vector<Point> > hulls(1);
                      vector<vector<int> > hullsI(1);
                      convexHull(Mat(tcontours[0]),hulls[0],false);
                      convexHull(Mat(tcontours[0]),hullsI[0],false);
                      drawContours(src,hulls,-1,cv::Scalar(0,255,0),2);
                      //This ratio indicates how wide are fingers opened
                      cont2hullRatio = (contourArea(hulls[0]) / currContArea);

                      vector<Vec4i> defects;
                      convexityDefects(tcontours[0], hullsI[0], defects);
                      float minDepth = 0.6f; // min acceptable convexity defect depth
                      float maxDepth = 1.9f; // max acceptable convexity defect depth
                      int maxIter    = 3;    // Number of reclustering iterations
                      vector<Point> goodDefects;
                      vector<float> goodDefectsDEPTH;
                      float dist = sqrt(contourArea(contours[i])) / 3.141f; // initial guess for limits
                      #ifdef GESTURE_VERBOSE
                        cout << "initial defects size is: " << defects.size() << " init_dist= " << dist << endl;
                      #endif
                      /* Average ALL depth points to get hand center */
                      int inflt   = 0;
                      for(int jj = 0; jj < maxIter; jj ++)
                      {
                        /* Remove outliers and recluster! */
                        float distflt = 0.0f;
                        inflt = 0;
                        for (unsigned int k = 0; k < defects.size(); k++) {
                             int fidx = defects[k][2];
                             Point pt( tcontours[0][fidx] );
                             //Do not take into account points close to border
                             if((pt.x < 15) || (frame.cols - pt.x) < 15) continue;
                             if((pt.y < 15) || (frame.rows - pt.y) < 15) continue;
                             float tmp_depth = defects[k][3] / 256.0f;
                             if(tmp_depth < minDepth * dist) continue;
                             if(tmp_depth > maxDepth * dist) continue;
                             {  
                               distflt += tmp_depth;
                               inflt ++;
                               if(jj == (maxIter - 1)) {
                                 goodDefects.push_back(pt);           
                                 goodDefectsDEPTH.push_back(tmp_depth);           
                               }
                             }
                        }
                        if(inflt > 0) {
                          dist = 0.5f * (minDepth + maxDepth) * (distflt / inflt);
                        }
                      }
                      #ifdef GESTURE_VERBOSE
                        cout << "List of defects, final dist=" << dist << endl;
                      #endif
                      float xx = 0.0, yy = 0.0;
                      for (unsigned int k = 0; k < goodDefects.size(); k ++)
                      {
                        circle( src, goodDefects[k], 6, Scalar(200,200,0), 3 );
                        xx += goodDefects[k].x;
                        yy += goodDefects[k].y;
                        #ifdef GESTURE_VERBOSE
                          cout << k << ")= (" << goodDefects[k].x << ", " << goodDefects[k].y << "), depth=" << goodDefectsDEPTH[k] << endl;
                        #endif
                      }
                      #ifdef GESTURE_VERBOSE
                        cout << "---end of defects---" << endl;
                      #endif
                      xx /= goodDefects.size(); 
                      yy /= goodDefects.size(); 
                      circle(src, Point(xx, yy), 5, Scalar(144,44,255),3);
               }
         }

        if(backgroundFrame>0) {
          char show_status[160];
#ifdef USE_CONTRAST_ADAPTATION
          snprintf (show_status, 160, "Background Rec #%3d contrast(%d)", backgroundFrame, needContrast);
#else
          snprintf (show_status, 160, "Background Rec #%3d", backgroundFrame);
#endif
          putText(src, show_status, cvPoint(16,16), FONT_HERSHEY_COMPLEX_SMALL, 1.0, cvScalar(200,200,250), 1, CV_AA);
          retVal = 0;
        } else {
          int  finger_votes[5], finger_vote_max = 0, finger_smooth = -1;
          char show_status[160];
          snprintf (show_status, 160, "contour2hull Ratio:%5.3f contrast(%d)", cont2hullRatio, needContrast );
          putText(src, show_status, cvPoint(24,24), FONT_HERSHEY_COMPLEX_SMALL, 1.0, cvScalar(200,200,250), 1, CV_AA);
          if(cont2hullRatio < 1.05f) retVal = 0;

#ifdef USE_CONTRAST_ADAPTATION
          else if(cont2hullRatio < 1.22f) retVal = 1;
          else if(cont2hullRatio < 1.50f) retVal = 3;
#else
          else if(cont2hullRatio < 1.30f) retVal = 1;
          else if(cont2hullRatio < 1.60f) retVal = 3;
#endif
          else retVal = 5;
          finger_window[FINGER_WINDOW_SIZE - 1] = retVal;
          for (int i = 0; i < 5; i++) finger_votes[i] = 0;
          for (int i = 0; i < FINGER_WINDOW_SIZE; i++) {
            if(finger_window[i] >= 1) finger_votes[finger_window[i] - 1] ++;
          }
          for(int i = 0; i < 5; i ++) {
            if(finger_vote_max < finger_votes[i]) {
              finger_vote_max = finger_votes[i];
              finger_smooth = i + 1;
            }
          }
          retVal = max(finger_smooth, 0);
        }
        src.copyTo(dst);

        return retVal;
}
/*** nothing past this point ***/

