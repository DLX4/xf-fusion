// File Description
// Author: Philip Salvaggio
//#include <opencv2/highgui/highgui.hpp>
#include "gaussian_pyramid.h"
#include "laplacian_pyramid.h"
#include "opencv_utils.h"
#include "remapping_function.h"

#include <iostream>
#include <sstream>

using namespace std;

void OutputBinaryImage(const std::string& filename, cv::Mat image) {
  FILE* f = fopen(filename.c_str(), "wb");
  for (int x = 0; x < image.cols; x++) {
    for (int y = 0; y < image.rows; y++) {
      double tmp = image.at<double>(y, x);
      fwrite(&tmp, sizeof(double), 1, f);
    }
  }
  fclose(f);
}

// Perform Local Laplacian filtering on the given image.
//
// Arguments:
//  input    The input image. Can be any type, but will be converted to double
//           for computation.
//  alpha    Exponent for the detail remapping function. (< 1 for detail
//           enhancement, > 1 for detail suppression)
//  beta     Slope for edge remapping function (< 1 for tone mapping, > 1 for
//           inverse tone mapping)
//  sigma_r  Edge threshold (in image range space).

cv::Mat FasterLocalLaplacianFilter(const cv::Mat& input,
                             double alpha,
                             double beta,
                             double sigma_r) {
  RemappingFunction r(alpha, beta);

  int num_levels = LaplacianPyramid::GetLevelCount(input.rows, input.cols, 30);
  cout << "Number of levels: " << num_levels << endl;

  const int kRows = input.rows;
  const int kCols = input.cols;

  GaussianPyramid gauss_input(input, num_levels);

  cv::Mat input_int = input * 255;
  GaussianPyramid gauss_input_int(input_int, num_levels);

  // Construct the unfilled Laplacian pyramid of the output. Copy the residual
  // over from the top of the Gaussian pyramid.
  LaplacianPyramid output(kRows, kCols, input.channels(), num_levels);
  gauss_input[num_levels].copyTo(output[num_levels]);

  for (int M = 0; M < 256; M++) {
      cout << ">>[0]" << M << endl;
      // Remap the region around the current pixel.
      cv::Mat r0;
      input.copyTo(r0);
      cv::Mat remapped;
      r.Evaluate<cv::Vec3d>(r0, remapped, (double)M/255, sigma_r);
      cout << ">>[1]" << M << endl;
      // Construct the Laplacian pyramid for the remapped region and copy the
      // coefficient over to the ouptut Laplacian pyramid.
      LaplacianPyramid tmp_pyr(remapped, num_levels + 1);

      cout << ">>[2]" << M << endl;
      // Calculate each level of the ouput Laplacian pyramid.
      for (int l = 0; l < num_levels; l++) {

        for (int y = 0; y < output[l].rows; y++) {

          for (int x = 0; x < output[l].cols; x++) {

            if ((gauss_input_int[l].at<cv::Vec3b>(y, x))[0] == M) {
                output.at<cv::Vec3d>(l, y, x)[0] = tmp_pyr.at<cv::Vec3d>(l, y, x)[0];
            }

            if ((gauss_input_int[l].at<cv::Vec3b>(y, x))[1] == M) {
                output.at<cv::Vec3d>(l, y, x)[1] = tmp_pyr.at<cv::Vec3d>(l, y, x)[1];
            }

            if ((gauss_input_int[l].at<cv::Vec3b>(y, x))[2] == M) {
                output.at<cv::Vec3d>(l, y, x)[2] = tmp_pyr.at<cv::Vec3d>(l, y, x)[2];
            }

          }

        }

      }

      cout << "<" << M << endl;
  }

  for (int l = 0; l < num_levels; l++) {
      stringstream ss;
      ss << "level" << l << ".png";
      cv::imwrite(ss.str(), ByteScale(cv::abs(output[l])));
      cout << endl;
  }

  return output.Reconstruct();
}

int main(int argc, char** argv) {
  const double kSigmaR = 0.3;
  const double kAlpha = 1;
  const double kBeta = 0;

  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " image_file" << endl;
    return 1;
  }

  cv::Mat input = cv::imread(argv[1]);
  if (input.data == NULL) {
    cerr << "Could not read input image." << endl;
    return 1;
  }
  imwrite("original.png", input);

  input.convertTo(input, CV_64F, 1 / 255.0);

  cout << "Input image: " << argv[1] << " Size: " << input.cols << " x "
       << input.rows << " Channels: " << input.channels() << endl;

  cv::Mat output;
  if (input.channels() == 1) {
    // output = LocalLaplacianFilter<double>(input, kAlpha, kBeta, kSigmaR);
  } else if (input.channels() == 3) {
    output = FasterLocalLaplacianFilter(input, kAlpha, kBeta, kSigmaR);
  } else {
    cerr << "Input image must have 1 or 3 channels." << endl;
    return 1;
  }

  output *= 255;
  output.convertTo(output, input.type());

  imwrite("output.png", output);

  return 0;
}
