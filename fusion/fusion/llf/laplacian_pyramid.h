// Class to represent a Laplacian pyramid of an image. The implementation is
// based on
//
// BURT, P. J., AND ADELSON, E. H. 1983. The Laplacian pyramid as a compact
// image code. IEEE Transactions on Communication 31, 4, 532–540.
//
// Author: Philip Salvaggio

#ifndef LAPLACIAN_PYRAMID_H
#define LAPLACIAN_PYRAMID_H

#include <opencv2/opencv.hpp>

class LaplacianPyramid {
 public:
  // Construct a blank Laplacian pyramid to be filled in by the user.
  //
  // Arguments:
  //  rows        The number of rows in the base level.
  //  cols        The number of columns of the base level.
  //  channels    The number of channels in the represented image.
  //  num_levels  The number of levels of the pyramid (excluding the top, which
  //              is the residual, or top of the Gaussian pyramid)
  LaplacianPyramid(int rows, int cols, int num_levels);
  LaplacianPyramid(int rows, int cols, int channels, int num_levels);

  // Construct the Laplacian pyramid of an image.
  //
  // Arguments:
  //  image      The input image. Can be any data type, but will be converted
  //             to double. Can be either 1 or 3 channels.
  //  num_levels The number of levels for the pyramid (excluding the top, which
  //             is the residual, or top of the Gaussian pyramid)
  //  subwindow  If this is a subimage [start_row, end_row, start_col, end_col]
  //             Both ends are inclusive.
  LaplacianPyramid(const cv::Mat& image, int num_levels);
  LaplacianPyramid(const cv::Mat& image, int num_levels,
                   const std::vector<int>& subwindow);

  // Move constructor if you want STL containers using emplace_back().
  LaplacianPyramid(LaplacianPyramid&& other);

  // No copying or assigning (too much memory footprint).
  LaplacianPyramid(const LaplacianPyramid&) = delete;
  LaplacianPyramid& operator=(const LaplacianPyramid&) = delete;

  // Get a level of the pyramid.
  const cv::Mat& operator[](int level) const { return pyramid_[level]; }
  cv::Mat& operator[](int level) { return pyramid_[level]; }

  // Element access.
  template<typename T>
  T& at(int level, int row, int col) {
    return pyramid_[level].at<T>(row, col);
  }

  // Reconstruct the image from the pyramid.
  cv::Mat Reconstruct() const;

  // Get the recommended number of levels given the input size and the desired
  // size of the residual image.
  static int GetLevelCount(int rows, int cols, int desired_base_size);

  // Output operator. Outputs level sizes.
  friend std::ostream &operator<<(std::ostream &output,
                                  const LaplacianPyramid& pyramid);

 private:
  std::vector<cv::Mat> pyramid_;
  std::vector<int> subwindow_;
};

#endif  // LAPLACIAN_PYRAMID_H
