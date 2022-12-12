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
void Add_image(cv::Mat &origin_img, cv::Mat &compare_img, cv::Mat &output_img);
void DCT_transform(cv::Mat &src, cv::Mat &output, int size=8);
void IDCT_transform(cv::Mat &src, cv::Mat &output, int size=8);
void DCT2D(cv::Mat &src, cv::Mat &output, int size);
void IDCT2D(cv::Mat &src, cv::Mat &output, int size);
void IDCT2D(cv::Mat &src, cv::Mat &output, int size);
void zigzag_limit(cv::Mat &src, cv::Mat &output, int size = 8,  int keep_num = 4);
void zigzag_remove(cv::Mat &src, cv::Mat &output,int size = 8, int keep_num = 4);
void Quantizer(cv::Mat &src, cv::Mat &output,int size = 8);
void zigzag_scan(cv::Mat &src,std::vector<float> &zigzag_1Darray, int size = 8, int get_num =4, bool read_or_write = true);
void reconstruction(cv::Mat &predict_img, cv::Mat &compressed_error_image, cv::Mat &output);

#endif
