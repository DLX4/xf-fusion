// File Description
// Author: Philip Salvaggio

#include "laplacian_pyramid.h"
#include "gaussian_pyramid.h"
#include <iostream>

using namespace std;
using cv::Mat;
using cv::Vec3d;

LaplacianPyramid::LaplacianPyramid(int rows, int cols, int num_levels)
    : LaplacianPyramid(rows, cols, 1, num_levels) {}

LaplacianPyramid::LaplacianPyramid(int rows,
                                   int cols,
                                   int channels,
                                   int num_levels)
    : pyramid_(), subwindow_({0, rows - 1, 0, cols - 1}) {
  pyramid_.reserve(num_levels + 1);
  for (int i = 0; i < num_levels + 1; i++) {
    pyramid_.emplace_back(ceil(rows / (double)(1 << i)),
                          ceil(cols / (double)(1 << i)), CV_64FC(channels));
  }
}

LaplacianPyramid::LaplacianPyramid(const Mat& image, int num_levels)
    : LaplacianPyramid(image, num_levels, {0, image.rows - 1,
                                           0, image.cols - 1}) {}

LaplacianPyramid::LaplacianPyramid(const Mat& image, int num_levels,
                                   const std::vector<int>& subwindow) 
    : pyramid_(), subwindow_(subwindow) {
  pyramid_.reserve(num_levels + 1);

  Mat input;
  image.convertTo(input, CV_64F);

  GaussianPyramid gauss_pyramid(input, num_levels, subwindow_);
  for (int i = 0; i < num_levels; i++) {
    pyramid_.emplace_back(gauss_pyramid[i] - gauss_pyramid.Expand(i + 1, 1));
  }
  pyramid_.emplace_back(gauss_pyramid[num_levels]);
}

LaplacianPyramid::LaplacianPyramid(LaplacianPyramid&& other)
    : pyramid_(std::move(other.pyramid_)) {}

Mat LaplacianPyramid::Reconstruct() const {
  Mat base = pyramid_.back();
  Mat expanded;

  for (int i = pyramid_.size() - 2; i >= 0; i--) {
    vector<int> subwindow;
    GaussianPyramid::GetLevelSize(subwindow_, i, &subwindow);
    int row_offset = ((subwindow[0] % 2) == 0) ? 0 : 1;
    int col_offset = ((subwindow[2] % 2) == 0) ? 0 : 1;

    expanded.create(pyramid_[i].rows, pyramid_[i].cols, base.type());

    if (base.channels() == 1) {
      GaussianPyramid::Expand<double>(base, row_offset, col_offset, expanded);
    } else if (base.channels() == 3) {
      GaussianPyramid::Expand<Vec3d>(base, row_offset, col_offset, expanded);
    }
    base = expanded + pyramid_[i];
  }

  return base;
}

int LaplacianPyramid::GetLevelCount(int rows, int cols, int desired_base_size) {
  int min_dim = std::min(rows, cols);

  double log2_dim = std::log2(min_dim);
  double log2_des = std::log2(desired_base_size);

  return static_cast<int>(std::ceil(std::abs(log2_dim - log2_des)));
}

std::ostream &operator<<(std::ostream &output,
                         const LaplacianPyramid& pyramid) {
  output << "Laplacian Pyramid:" << std::endl;
  for (size_t i = 0; i < pyramid.pyramid_.size(); i++) {
    output << "Level " << i << ": " << pyramid.pyramid_[i].cols << " x "
           << pyramid.pyramid_[i].rows;
    if (i != pyramid.pyramid_.size() - 1) output << std::endl;
  }
  return output;
}
