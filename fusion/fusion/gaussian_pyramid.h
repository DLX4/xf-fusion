// Class to represent a Gaussian pyramid of an image. The implementation is
// based on
//
// BURT, P. J., AND ADELSON, E. H. 1983. The Laplacian pyramid as a compact
// image code. IEEE Transactions on Communication 31, 4, 532–540.
//
// The 5x5 filter uses a=0.4, giving an approximate Gaussian.
// Author: Philip Salvaggio

#ifndef GAUSSIAN_PYRAMID_H
#define GAUSSIAN_PYRAMID_H

#include <opencv2/opencv.hpp>
#include <iostream>

class GaussianPyramid {
 public:
  // Construct a Gaussian pyramid of the given image. The number of levels does
  // not count the base, which is just the given image. So, the pyramid will
  // end up having num_levels + 1 levels. The image is converted to 64-bit
  // floating point for calculations.
  GaussianPyramid(const cv::Mat& image, int num_levels);

  // Indicates that this is a subimage. If the start index is odd, this is
  // necessary to make the higher levels the correct size.
  GaussianPyramid(const cv::Mat& image, int num_levels,
                  const std::vector<int>& subwindow);

  // Move constructor for having STL containers of GaussianPyramids.
  GaussianPyramid(GaussianPyramid&& other);

  // No copying or assigning.
  GaussianPyramid(const GaussianPyramid&) = delete;
  GaussianPyramid& operator=(const GaussianPyramid&) = delete;

  const cv::Mat& operator[](int level) const { return pyramid_[level]; }

  // Expand the given level a set number of times. The argument times must be
  // less than or equal to level, since the pyramid is used to determine the
  // size of the output. Having level equal to times will upsample the image to
  // the initial pixel dimensions.
  cv::Mat Expand(int level, int times) const;

  template<typename T>
  static void Expand(const cv::Mat& input,
                     int row_offset,
                     int col_offset,
                     cv::Mat& output);

  // Output operator, prints level sizes.
  friend std::ostream &operator<<(std::ostream &output,
                                  const GaussianPyramid& pyramid);

  static void GetLevelSize(const std::vector<int> base_subwindow,
                           int level,
                           std::vector<int>* subwindow);
 private:
  template<typename T>
  void PopulateTopLevel(int row_offset, int col_offset);

  // i = -2, -1, 0, 1, 2
  // a = 0.3 - Broad blurring Kernel
  // s = 0.4   Gaussian-like kernel
  // a = 0.5 - Triangle
  // a = 0.6 - Trimodal (Negative lobes)
  static double WeightingFunction(int i, double a);

  void GetLevelSize(int level, std::vector<int>* subwindow) const;

  constexpr static const double kA = 0.4;

 private:
  std::vector<cv::Mat> pyramid_;
  std::vector<int> subwindow_;
};

template<typename T>
void GaussianPyramid::PopulateTopLevel(int row_offset, int col_offset) {
  cv::Mat& previous = pyramid_[pyramid_.size() - 2];
  cv::Mat& top = pyramid_.back();
  
  // Calculate the end indices, based on where (0,0) is centered on the
  // previous level.
  const int kEndRow = row_offset + 2 * top.rows;
  const int kEndCol = col_offset + 2 * top.cols;
  for (int y = row_offset; y < kEndRow; y += 2) {
    for (int x = col_offset; x < kEndCol; x += 2) {
      T value = 0;
      double total_weight = 0;
      
      int row_start = std::max(0, y - 2);
      int row_end = std::min(previous.rows - 1, y + 2);
      for (int n = row_start; n <= row_end; n++) {
        double row_weight = WeightingFunction(n - y, kA);

        int col_start = std::max(0, x - 2);
        int col_end = std::min(previous.cols - 1, x + 2);
        for (int m = col_start; m <= col_end; m++) {
          double weight = row_weight * WeightingFunction(m - x, kA);
          total_weight += weight;
          value += weight * previous.at<T>(n, m);
        }
      }
      top.at<T>(y >> 1, x >> 1) = value / total_weight;
    }
  }
}

template<typename T>
void GaussianPyramid::Expand(const cv::Mat& input,
                             int row_offset,
                             int col_offset,
                             cv::Mat& output) {
  cv::Mat upsamp = cv::Mat::zeros(output.rows, output.cols, input.type());
  cv::Mat norm = cv::Mat::zeros(output.rows, output.cols, CV_64F);

  for (int i = row_offset; i < output.rows; i += 2) {
    for (int j = col_offset; j < output.cols; j += 2) {
      upsamp.at<T>(i, j) = input.at<T>(i >> 1, j >> 1);
      norm.at<double>(i, j) = 1;
    }
  }

  cv::Mat filter(5, 5, CV_64F);
  for (int i = -2; i <= 2; i++) {
    for (int j = -2; j <= 2; j++) {
      filter.at<double>(i + 2, j + 2) =
         WeightingFunction(i, kA) * WeightingFunction(j, kA);
    }
  }

  for (int i = 0; i < output.rows; i++) {
    int row_start = std::max(0, i - 2);
    int row_end = std::min(output.rows - 1, i + 2);
    for (int j = 0; j < output.cols; j++) {
      int col_start = std::max(0, j - 2);
      int col_end = std::min(output.cols - 1, j + 2);

      T value = 0;
      double total_weight = 0;
      for (int n = row_start; n <= row_end; n++) {
        for (int m = col_start; m <= col_end; m++) {
          double weight = filter.at<double>(n - i + 2, m - j + 2);
          value += weight * upsamp.at<T>(n, m);
          total_weight += weight * norm.at<double>(n, m);
        }
      }
      output.at<T>(i, j) = value / total_weight;
    }
  }
}

#endif  // GAUSSIAN_PYRAMID_H
