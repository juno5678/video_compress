#ifndef __EBMA_LIB_H__
#define __EBMA_LIB_H__

#include <iostream>
#include <math.h>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <time.h>
#include <chrono>
#include <vector>

void EBMA(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size);
int Check_index_boundry(int index,int upper_boundry,int low_boundry);
void Set_MV_color(cv::Mat &MV,cv::Point &mv,cv::Point &set_pos,short  block_size);
double Cal_EOF(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &mv_temp,cv::Point &block_pos,short block_size,short p=2);
void Predict_ref_frame(cv::Mat &predict_frame,cv::Mat &ref_frame,cv::Point &mv,cv::Point &block_pos,short block_size);
double Cal_EDFD(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &mv_temp,cv::Point &block_pos,short block_size,short p=1);


#endif
