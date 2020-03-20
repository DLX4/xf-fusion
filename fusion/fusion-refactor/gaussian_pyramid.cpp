// File Description
// Author: Philip Salvaggio

#include "gaussian_pyramid.h"
#include <iostream>

using namespace std;
using cv::Mat;
using cv::Vec3d;

GaussianPyramid::GaussianPyramid(const Mat& image, int num_levels)
    : GaussianPyramid(image, num_levels, {0, image.rows - 1,
                                          0, image.cols - 1}) {}

GaussianPyramid::GaussianPyramid(GaussianPyramid&& other)
    : pyramid_(move(other.pyramid_)) {}

GaussianPyramid::GaussianPyramid(const Mat& image, int num_levels,
                                 const vector<int>& subwindow)
    : pyramid_(), subwindow_(subwindow) {
  pyramid_.reserve(num_levels + 1);
  pyramid_.emplace_back();
  image.convertTo(pyramid_.back(), CV_64F);

  // This test verifies that the image is large enough to support the requested
  // number of levels.
  if (image.cols >> num_levels == 0 || image.rows >> num_levels == 0) {
    cerr << "Warning: Too many levels requested. Image size " 
         << image.cols << " x " << image.rows << " and  " << num_levels 
         << " levels wer requested." << endl;
  }

  for (int l = 0; l < num_levels; l++) {
    const Mat& previous = pyramid_.back();

    // Get the subwindows of the previous level and the current one.
    vector<int> prev_subwindow, current_subwindow;
    GetLevelSize(pyramid_.size() - 1, &prev_subwindow);
    GetLevelSize(pyramid_.size(), &current_subwindow);

    const int kRows = current_subwindow[1] - current_subwindow[0] + 1;
    const int kCols = current_subwindow[3] - current_subwindow[2] + 1;

    // If the subwindow starts on even indices, then (0,0) of the new level is
    // centered on (0,0) of the previous level. Otherwise, it's centered on
    // (1,1).
    int row_offset = ((prev_subwindow[0] % 2) == 0) ? 0 : 1;
    int col_offset = ((prev_subwindow[2] % 2) == 0) ? 0 : 1;

    // Push a new level onto the top of the pyramid.
    pyramid_.emplace_back(kRows, kCols, previous.type());
    Mat& next = pyramid_.back();

    // Populate the next level.
    if (next.channels() == 1) {
      PopulateTopLevel<double>(row_offset, col_offset);
    } else if (next.channels() == 3) {
      PopulateTopLevel<Vec3d>(row_offset, col_offset);
    }
  }
}


Mat GaussianPyramid::Expand(int level, int times) const {
  if (times < 1) return pyramid_.at(level);
  times = min(times, level);

  Mat base = pyramid_[level], expanded;

  for (int i = 0; i < times; i++) {
    vector<int> subwindow;
    GetLevelSize(level - i - 1, &subwindow);

    int out_rows = pyramid_[level - i - 1].rows;
    int out_cols = pyramid_[level - i - 1].cols;
    expanded.create(out_rows, out_cols, base.type());

    int row_offset = ((subwindow[0] % 2) == 0) ? 0 : 1;
    int col_offset = ((subwindow[2] % 2) == 0) ? 0 : 1;
    if (base.channels() == 1) {
      Expand<double>(base, row_offset, col_offset, expanded);
    } else {
      Expand<Vec3d>(base, row_offset, col_offset, expanded);
    }

    base = expanded;
  }

  return expanded;
}


ostream &operator<<(ostream &output, const GaussianPyramid& pyramid) {
  output << "Gaussian Pyramid:" << endl;
  for (size_t i = 0; i < pyramid.pyramid_.size(); i++) {
    output << "Level " << i << ": " << pyramid.pyramid_[i].cols << " x "
           << pyramid.pyramid_[i].rows;
    if (i != pyramid.pyramid_.size() - 1) output << endl;
  }
  return output;
}

void GaussianPyramid::GetLevelSize(int level, vector<int>* subwindow) const {
  GetLevelSize(subwindow_, level, subwindow);
}

void GaussianPyramid::GetLevelSize(const vector<int> base_subwindow,
                                   int level,
                                   vector<int>* subwindow) {
  subwindow->clear();
  subwindow->insert(begin(*subwindow),
      begin(base_subwindow), end(base_subwindow));

  for (int i = 0; i < level; i++) {
    (*subwindow)[0] = ((*subwindow)[0] >> 1) + (*subwindow)[0] % 2;
    (*subwindow)[1] = (*subwindow)[1] >> 1;
    (*subwindow)[2] = ((*subwindow)[2] >> 1) + (*subwindow)[2] % 2;
    (*subwindow)[3] = (*subwindow)[3] >> 1;
  }
}

inline double GaussianPyramid::WeightingFunction(int i, double a) {
  switch (i) {
    case 0: return a;
    case -1: case 1: return 0.25;
    case -2: case 2: return 0.25 - 0.5 * a;
  }
  return 0;
}
