#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

void calcul_histogramme_BW(cv::Mat& img, int* hist) {

    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            unsigned char value = img.at<unsigned char>(y, x);
            hist[value]++;
        }
    }
}

void calcul_histogramme_RGB(cv::Mat& img, int* hist_R, int* hist_G, int* hist_B) {

    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);
            hist_R[pixel[2]]++;
            hist_G[pixel[1]]++;
            hist_B[pixel[0]]++;
        }
    }
}

int main() {
    cv::Mat img = cv::imread("./test.jpg");
    if (img.empty()) return 1;

    int histR[256] = { 0 };
    int histG[256] = { 0 };
    int histB[256] = { 0 };

    calcul_histogramme_RGB(img, histR, histG, histB);

    // for (int y = 0; y < 1; ++y) {
    //     for (int x = 0; x < 1; ++x) {
    //         uchar value = img.at<uchar>(y, x);
    //         std::cout << (int)value << ",";
    //     }
    //     std::cout << std::endl;
    // }

    for (int y : histR) {
        std::cout << y << " ";
    }

    std::cout << std::endl;

    for (int y : histG) {
        std::cout << y << " ";
    }

    std::cout << std::endl;

    for (int y : histB) {
        std::cout << y << " ";
    }

    cv::imshow("test", img);
    cv::waitKey(0);
    return 0;
}