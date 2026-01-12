#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

void histogramme(cv::Mat& img, int* hist) {

    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            uchar value = img.at<uchar>(y, x);
            hist[value]++;
        }
    }
}

int main() {
    cv::Mat img = cv::imread("./test.jpg");
    if (img.empty()) return 1;

    int hist[256] = { 0 };

    histogramme(img, hist);

    for (int y = 0; y < 256; ++y) {
        std::cout << hist[y] << std::endl;
    }

    cv::imshow("test", img);
    cv::waitKey(0);
    return 0;
}