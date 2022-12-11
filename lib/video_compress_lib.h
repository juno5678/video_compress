#ifndef __VIDEO_COMPROCESS_LIB_H__
#define __VIDEO_COMPROCESS_LIB_H__

#include <iostream>
#include <math.h>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <time.h>
#include <chrono>
#include <vector>

double Cal_MSE(const cv::Mat &predict_frame,const cv::Mat &target_frame);
void Get_chrominance(cv::Mat &origin_img,cv::Mat &chrom_img);
double Cal_PSNR(double mse,int max_i=255);
void Get_error_image(cv::Mat &origin_img, cv::Mat &compare_img, cv::Mat &error_img);
void DCT_transform(cv::Mat &src, cv::Mat &output, int size=8);


#endif
