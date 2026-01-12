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

cv::Mat afficherHistogramme(int* hist) {
    int histSize = 256;
    int histH = 200;
    int histW = histSize;

    // Image blanche
    cv::Mat histImage(histH, histW, CV_8UC1, cv::Scalar(255));

    // Trouver la valeur max pour normalisation
    int maxVal = 0;
    for(int i = 0; i < histSize; i++)
        if(hist[i] > maxVal) maxVal = hist[i];

    // Dessiner les barres (noir, inversé)
    for(int i = 0; i < histSize; i++) {
        int barHeight = (int)(((float)hist[i] / maxVal) * histH);
        for(int y = histH - barHeight; y < histH; y++) {  // inversé
            histImage.at<uchar>(y, i) = 0;  // noir
        }
    }

    return histImage;
}

int main() {
    cv::Mat img = cv::imread("./lena_gray.gif");
    if (img.empty()) return 1;

    int hist[256] = { 0 };

    histogramme(img, hist);

    for (int y = 0; y < 256; ++y) {
        std::cout << hist[y] << std::endl;
    }

    cv::Mat histImg = afficherHistogramme(hist);
    cv::imshow("Histogramme", histImg);
    cv::waitKey(0);

    cv::imshow("test", img);
    cv::waitKey(0);
    return 0;
}