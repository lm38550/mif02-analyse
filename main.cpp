#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

int main() {
    cv::Mat img = cv::imread("./test.jpg");
    if (img.empty()) return 1;
    cv::imshow("test", img);
    cv::waitKey(0);
    return 0;
}