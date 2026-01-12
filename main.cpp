#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
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

// ******************* 5.1 ********************
cv::Mat inverserImageGris(const cv::Mat& img) {
    cv::Mat inv = img.clone();

    for(int y = 0; y < img.rows; y++) {
        for(int x = 0; x < img.cols; x++) {
            inv.at<uchar>(y, x) = 255 - img.at<uchar>(y, x);
        }
    }

    return inv;
}

cv::Mat inverserImageRGB(const cv::Mat& img) {
    cv::Mat inv = img.clone();

    for(int y = 0; y < img.rows; y++) {
        for(int x = 0; x < img.cols; x++) {
            cv::Vec3b pix = img.at<cv::Vec3b>(y, x);
            inv.at<cv::Vec3b>(y, x)[0] = 255 - pix[0]; // B
            inv.at<cv::Vec3b>(y, x)[1] = 255 - pix[1]; // G
            inv.at<cv::Vec3b>(y, x)[2] = 255 - pix[2]; // R
        }
    }

    return inv;
}

// ******************* 5.3 ********************
cv::Mat egaliserHistogramme(const cv::Mat& img) {
    cv::Mat result = img.clone();
    int hist[256] = {0};
    int cdf[256] = {0};

    int totalPixels = img.rows * img.cols;

    // 1. Calculer histogramme
    for(int y = 0; y < img.rows; y++)
        for(int x = 0; x < img.cols; x++)
            hist[img.at<uchar>(y,x)]++;

    // 2. Calculer CDF cumulée
    cdf[0] = hist[0];
    for(int i = 1; i < 256; i++)
        cdf[i] = cdf[i-1] + hist[i];

    // 3. Normaliser CDF et appliquer transformation
    for(int y = 0; y < img.rows; y++) {
        for(int x = 0; x < img.cols; x++) {
            int v = img.at<uchar>(y,x);
            int v_new = (int)((float)cdf[v] / totalPixels * 255.0);
            result.at<uchar>(y,x) = v_new;
        }
    }

    return result;
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
    // 1. Lire l'image
    cv::Mat img = cv::imread("./cathedrale.jpg");
    cv::Mat img = cv::imread("./cathedrale.jpg");
    if (img.empty()) return 1;

    int hist1[256] = { 0 };
    int hist2[256] = { 0 };

    calcul_histogramme_BW(img, hist1);

    cv::Mat histImg = afficherHistogramme(hist1);
    cv::imshow("Histogramme Original", histImg);
    cv::waitKey(0);
    // 2. Convertir en niveaux de gris
    cv::Mat gray;
    if(img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        gray = img.clone();

    cv::imshow("Image Original", img);
    cv::waitKey(0);

    extend_compress(img, 120, 250);
    calcul_histogramme_BW(img, hist2);

    cv::Mat histImg2 = afficherHistogramme(hist2);
    cv::imshow("Histogramme Compress", histImg2);
    cv::waitKey(0);
    // --- IMAGE ORIGINALE ---
    int hist[256] = {0};
    calcul_histogramme_BW(gray, hist);
    cv::Mat histImg = afficherHistogramme(hist);

    // --- IMAGE INVERSEE ---
    cv::Mat imgInversee = inverserImageGris(gray);
    int histInv[256] = {0};
    calcul_histogramme_BW(imgInversee, histInv);
    cv::Mat histImgInv = afficherHistogramme(histInv);

    // --- IMAGE EGALEE ---
    cv::Mat imgEgalisee = egaliserHistogramme(gray);
    int histEgal[256] = {0};
    calcul_histogramme_BW(imgEgalisee, histEgal);
    cv::Mat histImgEgal = afficherHistogramme(histEgal);

    // 4. Affichage de toutes les images et histogrammes
    cv::imshow("Image originale", gray);
    cv::imshow("Histogramme original", histImg);
    cv::imshow("Image inversée", imgInversee);
    cv::imshow("Histogramme inversé", histImgInv);
    cv::imshow("Image égalisée", imgEgalisee);
    cv::imshow("Histogramme égalisé", histImgEgal);

    cv::waitKey(0);
    return 0;
}