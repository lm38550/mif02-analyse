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

    cv::Mat histImg = afficherHistogramme(hist);
    cv::imshow("Histogramme", histImg);
    cv::waitKey(0);

    cv::imshow("test", img);
    cv::waitKey(0);
    return 0;
}