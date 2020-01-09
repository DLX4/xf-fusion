#include "mainwindow.h"

#include <QCoreApplication>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cv::Mat image = cv::imread("D:\\code\\dan-qt\\Demo\\image.jpg");
    cv::namedWindow("Image");
    cv::imshow("Image", image);

    cv::waitKey(0);
    return a.exec();
}

