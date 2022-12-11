#ifndef __FAST_BMA_LIB_H__
#define __FAST_BMA_LIB_H__

#include "EBMA_lib.h"
void BMA_2Dlog(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size);
void BMA_NTSS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size);
void Diamond_search(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size);
void Fast_Square_search(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size);
void Fst_step_NTSS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size);
void Square_search(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size);
void BMA_TSS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size);
void BMA_4SS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size);
#endif
