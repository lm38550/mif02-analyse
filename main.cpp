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

void extend_compress(cv::Mat& img, int newMin, int newMax)
{
    int channels = img.channels();

    for (int c = 0; c < channels; ++c) {
        int minVal = 255, maxVal = 0;

        for (int y = 0; y < img.rows; ++y) {
            for (int x = 0; x < img.cols; ++x) {
                int val = img.at<unsigned char>(y, x);
                minVal = std::min(minVal, val);
                maxVal = std::max(maxVal, val);
            }
        }

        // extension par canal
        for (int y = 0; y < img.rows; ++y) {
            unsigned char* row = img.ptr<unsigned char>(y);
            for (int x = c; x < img.cols * channels; x += channels) {
                int val = row[x];
                row[x] = (val - minVal) * (newMax - newMin) / (maxVal - minVal) + newMin;
            }
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
            histImage.at<unsigned char>(y, i) = 0;  // noir
        }
    }

    return histImage;
}

int main() {
    cv::Mat img = cv::imread("./cathedrale.jpg");
    if (img.empty()) return 1;

    int hist1[256] = { 0 };
    int hist2[256] = { 0 };

    calcul_histogramme_BW(img, hist1);

    cv::Mat histImg = afficherHistogramme(hist1);
    cv::imshow("Histogramme Original", histImg);
    cv::waitKey(0);

    cv::imshow("Image Original", img);
    cv::waitKey(0);

    extend_compress(img, 120, 250);
    calcul_histogramme_BW(img, hist2);

    cv::Mat histImg2 = afficherHistogramme(hist2);
    cv::imshow("Histogramme Compress", histImg2);
    cv::waitKey(0);

    cv::imshow("test", img);
    cv::waitKey(0);
    return 0;
}