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

cv::Mat extend_compress(cv::Mat& img, int newMin, int newMax)
{
    cv::Mat inv = img.clone();
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
        for (int y = 0; y < inv.rows; ++y) {
            unsigned char* row = inv.ptr<unsigned char>(y);
            for (int x = c; x < inv.cols * channels; x += channels) {
                int val = row[x];
                row[x] = (val - minVal) * (newMax - newMin) / (maxVal - minVal) + newMin;
            }
        }
    }
    return inv;
}

cv::Mat inverserImage(const cv::Mat& img) {
    cv::Mat inv = img.clone();
    int channels = img.channels();

    for (int c = 0; c < channels; ++c) {
        for(int y = 0; y < img.rows; y++) {
            for (int x = c; x < inv.cols * channels; x += channels) {
                inv.at<uchar>(y, x) = 255 - img.at<uchar>(y, x);
            }
        }
    }

    return inv;
}

cv::Mat egaliserHistogramme(const cv::Mat& img) {
    cv::Mat result = img.clone();
    int channels = img.channels();

    for (int c = 0; c < channels; ++c) {
        int hist[256] = {0};
        int cdf[256] = {0};

        int totalPixels = img.rows * img.cols;

        // 1. Calculer histogramme
        for(int y = 0; y < img.rows; y++) {
            for (int x = c; x < result.cols * channels; x += channels) {
                hist[img.at<uchar>(y,x)]++;
            }
        }

        // 2. Calculer CDF cumulée
        cdf[0] = hist[0];
        for(int i = 1; i < 256; i++)
            cdf[i] = cdf[i-1] + hist[i];

        // 3. Normaliser CDF et appliquer transformation
        for(int y = 0; y < img.rows; y++) {
            for (int x = c; x < result.cols * channels; x += channels) {
                int v = img.at<uchar>(y,x);
                int v_new = (int)((float)cdf[v] / totalPixels * 255.0);
                result.at<uchar>(y,x) = v_new;
            }
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
    // ---- Chargement de l'image Originale ----
    cv::Mat img = cv::imread("./cathedrale.jpg");
    if (img.empty()) return 1;

    // ---- Calcul de l'histogramme et affichages ----
    int hist_org[256] = { 0 };
    calcul_histogramme_BW(img, hist_org);

    cv::Mat histImg_org = afficherHistogramme(hist_org);
    cv::imshow("Histogramme Original", histImg_org);
    cv::waitKey(0);

    cv::imshow("Image Original", img);
    cv::waitKey(0);

    // ---- Calcul de l'image compressé ----
    cv::Mat img_compress = extend_compress(img, 120, 200);

    // ---- Calcul de l'histogramme et affichages ----
    int hist_compress[256] = { 0 };
    calcul_histogramme_BW(img_compress, hist_compress);

    cv::Mat histImg_compress = afficherHistogramme(hist_compress);
    cv::imshow("Histogramme Compress", histImg_compress);
    cv::waitKey(0);

    cv::imshow("Image Compress", img_compress);
    cv::waitKey(0);

    // ---- Calcul de l'image inverse ----
    cv::Mat img_inverse = inverserImage(img);

    // ---- Calcul de l'histogramme et affichages ----
    int hist_inverse[256] = { 0 };
    calcul_histogramme_BW(img_inverse, hist_inverse);

    cv::Mat histImg_inverse = afficherHistogramme(hist_inverse);
    cv::imshow("Histogramme Inverse", histImg_inverse);
    cv::waitKey(0);

    cv::imshow("Image Inverse", img_inverse);
    cv::waitKey(0);

    // ---- Calcul de l'image inverse ----
    cv::Mat img_egalise = egaliserHistogramme(img_compress);

    // ---- Calcul de l'histogramme et affichages ----
    int hist_egalise[256] = { 0 };
    calcul_histogramme_BW(img_egalise, hist_egalise);

    cv::Mat histImg_egalise = afficherHistogramme(hist_egalise);
    cv::imshow("Histogramme Egalise", histImg_egalise);
    cv::waitKey(0);

    cv::imshow("Image Egalise", img_egalise);
    cv::waitKey(0);


    // ---- Calcul de l'image en gris ----
    cv::Mat img_gris = convertirEnGris(img);

    // ---- Calcul de l'histogramme et affichages ----
    int hist_gris[256] = { 0 };
    calcul_histogramme_BW(img_gris, hist_gris);

    cv::Mat histImg_gris = afficherHistogramme(hist_gris);
    cv::imshow("Histogramme Gris", histImg_gris);
    cv::waitKey(0);

    cv::imshow("Image Gris", img_gris);
    cv::waitKey(0);

    return 0;
}