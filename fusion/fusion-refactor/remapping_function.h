// File Description
// Author: Philip Salvaggio

#ifndef REMAPPING_FUNCTION_H
#define REMAPPING_FUNCTION_H

#include <opencv2/opencv.hpp>
#include <cmath>

class RemappingFunction {
 public:
  RemappingFunction(double alpha, double beta);
  ~RemappingFunction();

  double alpha() const { return alpha_; }
  void set_alpha(double alpha) { alpha_ = alpha; }

  double beta() const { return beta_; }
  void set_beta(double beta) { beta_ = beta; }

  void Evaluate(double value,
                double reference,
                double sigma_r,
                double& output);
  void Evaluate(const cv::Vec3d& value,
                const cv::Vec3d& reference,
                double sigma_r,
                cv::Vec3d& output);

  template<typename T>
  void Evaluate(const cv::Mat& input, cv::Mat& output,
      const T& reference, double sigma_r);

 private:
  double DetailRemap(double delta, double sigma_r);
  double EdgeRemap(double delta);

  double SmoothStep(double x_min, double x_max, double x);

 private:
  double alpha_, beta_;
};

inline double RemappingFunction::DetailRemap(double delta, double sigma_r) {
  double fraction = delta / sigma_r;
  double polynomial = pow(fraction, alpha_);
  if (alpha_ < 1) {
    const double kNoiseLevel = 0.01;
    double blend = SmoothStep(kNoiseLevel,
        2 * kNoiseLevel, fraction * sigma_r);
    polynomial = blend * polynomial + (1 - blend) * fraction;
  }
  return polynomial;
}

inline double RemappingFunction::EdgeRemap(double delta) {
  return beta_ * delta;
}

template<typename T>
void RemappingFunction::Evaluate(const cv::Mat& input, cv::Mat& output,
      const T& reference, double sigma_r) {
  output.create(input.rows, input.cols, input.type());
  for (int i = 0; i < input.rows; i++) {
    for (int j = 0; j < input.cols; j++) {
      Evaluate(input.at<T>(i, j), reference, sigma_r, output.at<T>(i, j));
    }
  }
}

#endif  // REMAPPING_FUNCTION_H
