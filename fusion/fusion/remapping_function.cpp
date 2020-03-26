// File Description
// Author: Philip Salvaggio

#include "remapping_function.h"

#include <cmath>
#include <algorithm>

using namespace std;

RemappingFunction::RemappingFunction(double alpha, double beta)
    : alpha_(alpha), beta_(beta) {}

RemappingFunction::~RemappingFunction() {}

double RemappingFunction::SmoothStep(double x_min, double x_max, double x) {
  double y = (x - x_min) / (x_max - x_min);
  y = max(0.0, min(1.0, y));
  return pow(y, 2) * pow(y-2, 2);
}

void RemappingFunction::Evaluate(double value,
                                 double reference,
                                 double sigma_r,
                                 double& output) {
  double delta = std::abs(value - reference);
  int sign = value < reference ? -1 : 1;

  // rd(g0 ± sr) = re(g0 ± sr)
  if (delta < sigma_r) {
    // rd(i) = g0 + sign(i−g0)sr fd(ji−g0j=sr)

    // The function rd modifies the fine-scale details by altering the oscillations around the value g0 (reference).
    // DetailRemap is a smooth function mapping from [0;1] to [0;1] that controls how details are modified.  返回1时梯度最大
    output = reference + sign * sigma_r * DetailRemap(delta, sigma_r);
  } else {
    // re(i) = g0 +sign(i− g0) fe(ji− g0j− sr)+ sr
    // re is a function that modifies the amplitude of edges
    // fe is a smooth non-negative function defined over [0;¥).
    output = reference + sign * (EdgeRemap(delta - sigma_r) + sigma_r);
  }
}

void RemappingFunction::Evaluate(const cv::Vec3d& value,
                                 const cv::Vec3d& reference,
                                 double sigma_r,
                                 cv::Vec3d& output) {
  cv::Vec3d delta = value - reference;
  double mag = cv::norm(delta);
  if (mag > 1e-10) delta /= mag;

  if (mag < sigma_r) {
    output = reference + delta * sigma_r * DetailRemap(mag, sigma_r);
  } else {
    output = reference + delta * (EdgeRemap(mag - sigma_r) + sigma_r);
  }
}
