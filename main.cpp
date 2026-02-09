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
            histImage.at<uchar>(y, i) = 0;  // noir
        }
    }

    return histImage;
}

cv::Mat convertirEnGris(const cv::Mat& img) {
    cv::Mat gray(img.rows, img.cols, CV_8UC1);

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);

            uchar gris = static_cast<uchar>(
                0.299 * pixel[2] +   // R
                0.587 * pixel[1] +   // G
                0.114 * pixel[0]     // B
            );

            gray.at<uchar>(y, x) = gris;
        }
    }
    return gray;
}

int main() {
    // 1. Lire l'image
    cv::Mat img = cv::imread("./cathedrale.jpg");
    if (img.empty()) return 1;

    // 2. Convertir en niveaux de gris
    cv::Mat gray;
    if(img.channels() == 3)
        gray = convertirEnGris(img);
    else
        gray = img.clone();

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
