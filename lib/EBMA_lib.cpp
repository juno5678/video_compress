#include "EBMA_lib.h"
#include "opencv2/core.hpp"
#include <cmath>

static int Check_search_boundry(int now_pos,int search_size,int boundry)
{
    if(boundry == 0)
    {
        if(now_pos+search_size < boundry)
            return -now_pos;
    }
    else if(boundry >0 )
    {
        if(now_pos+search_size > boundry)
            return boundry-now_pos;
    }
    return search_size;
}
void Set_MV_color(cv::Mat &MV,cv::Point &mv,cv::Point &set_pos,short  block_size)
{
    cv::Mat colorSpace = cv::imread("../data/color_space2.png");
    if(colorSpace.empty())
    {
        printf("didn't found colos space\n");
        return;
    }

    cv::Point center(colorSpace.cols/2,colorSpace.rows/2);
    for(int i = 0 ; i < block_size;i++)
    {
        for(int j = 0 ; j < block_size;j++)
        {
            MV.at<cv::Vec3b>(set_pos.y+j,set_pos.x+i)[0] = colorSpace.at<cv::Vec3b>(std::round(center.y+mv.y),std::round(center.x+mv.x))[0];
            MV.at<cv::Vec3b>(set_pos.y+j,set_pos.x+i)[1] = colorSpace.at<cv::Vec3b>(center.y+mv.y,center.x+mv.x)[1];
            MV.at<cv::Vec3b>(set_pos.y+j,set_pos.x+i)[2] = colorSpace.at<cv::Vec3b>(center.y+mv.y,center.x+mv.x)[2];
        }
    }
    cv::imshow("color space",colorSpace);

}

int Check_index_boundry(int index,int upper_boundry,int low_boundry)
{
    if(index >= upper_boundry)
        return upper_boundry-1;
    else if(index <= low_boundry)
        return low_boundry;
    else
        return index;
}
double Cal_EOF(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &mv_temp,cv::Point &block_pos,short block_size,short p)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    double sum = 0;
    double OF_value = 0;
    for(int bx = 0 ; bx < block_size;bx++)
    {
        for(int by = 0 ; by < block_size;by++)
        {
            cv::Point now_pos(block_pos.x+bx,block_pos.y+by);
            int cur_I = current_frame.at<uchar>(now_pos);
            int del_x = current_frame.at<uchar>(now_pos.y,std::min(now_pos.x + 1,M_width-1 )) - cur_I;
            int del_y = current_frame.at<uchar>(std::min(now_pos.y+1,M_height-1),now_pos.x  )  - cur_I;
            int ref_I = ref_frame.at<uchar>(now_pos);
            int del_I = del_x*mv_temp.x + del_y*mv_temp.y;

            if(p == 1)
                OF_value = std::abs(del_I + ref_I -cur_I);
            else
                OF_value = std::sqrt( (del_I + ref_I -cur_I)*(del_I + ref_I -cur_I));

            sum += OF_value;
        }
    }
    return sum;

}
void Predict_ref_frame(cv::Mat &predict_frame,cv::Mat &ref_frame,cv::Point &mv,cv::Point &block_pos,short block_size)
{
    int M_width = ref_frame.cols;
    int M_height = ref_frame.rows;
    for(int bx = 0 ; bx < block_size;bx++)
    {
        for(int by = 0 ; by<block_size;by++)
        {
            cv::Point now_pos(block_pos.x+bx,block_pos.y+by);
            int predict_index_x = Check_index_boundry(now_pos.x+mv.x,M_width,0);
            int predict_index_y = Check_index_boundry(now_pos.y+mv.y,M_height,0);
            predict_frame.at<uchar>(now_pos) = ref_frame.at<uchar>(predict_index_y,predict_index_x);
        }
    }

}
double Cal_EDFD(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &mv_temp,cv::Point &block_pos,short block_size,short p)
{
    int sum = 0;
    double DFD_value = 0;
    for(int bx = 0 ; bx < block_size;bx++)
    {
        for(int by = 0 ; by < block_size;by++)
        {
            cv::Point now_pos(block_pos.x+bx,block_pos.y+by);
            cv::Point search_pos(now_pos.x+mv_temp.x,now_pos.y+mv_temp.y);
            int cur_I = current_frame.at<uchar>(now_pos);
            int ref_I = ref_frame.at<uchar>(search_pos);
            if(p == 2)
                DFD_value = std::sqrt((ref_I-cur_I)*(ref_I-cur_I));
            else
                DFD_value = std::abs(ref_I-cur_I);

            sum += DFD_value;
        }
    }
    return sum;

}
void EBMA(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    int candidate_cols = M_width/block_size;
    int candidate_rows = M_height/block_size;
    predict_frame = cv::Mat(current_frame.size(),current_frame.type());
    cv::Mat MV_line = cv::Mat::zeros(current_frame.size(),current_frame.type());
    cv::Mat MV_color = cv::Mat::zeros(current_frame.size(),CV_8UC3);
    int step_count = 0;

    for(int j = 0; j < candidate_rows;j++)
    {
        for(int i = 0;i < candidate_cols;i++)
        {
            double min = 10000;
            cv::Point MV(0,0);
            cv::Point block_pos(i*block_size,j*block_size);
            int n_search_point = 0;
            for(int x = Check_search_boundry(i*block_size,-search_size,0) ; x <= Check_search_boundry(i*block_size,search_size,M_width-1);x++)
            {
                for(int y = Check_search_boundry(j*block_size,-search_size,0) ; y <= Check_search_boundry(j*block_size,search_size,M_height-1);y++)
                {
                    cv::Point mv_temp(x,y);
                    double criterion = Cal_EOF(current_frame,ref_frame,mv_temp,block_pos,block_size);

                    if(criterion < min)
                    {
                        min = criterion;
                        MV = mv_temp;
                    }
                n_search_point++;
                }
            }
            if(j == candidate_rows/2 && i == candidate_cols/2)
                printf("n search point : %d\n",n_search_point);
            cv::Point global_MV(block_pos.x+MV.x,block_pos.y+MV.y);
            if(MV.x != 0 && MV.y != 0)
            {
                cv::circle(MV_line,global_MV,2,cv::Scalar(255),-1);
                //printf("block_pos x: %d , y: %d\n",block_pos.x,block_pos.y);
                //printf("gloval mv x: %d , y: %d\n",global_MV.x,global_MV.y);
            }
            cv::line(MV_line,block_pos,global_MV,cv::Scalar(255));
            Set_MV_color(MV_color,MV,block_pos,block_size);
            Predict_ref_frame(predict_frame,ref_frame,MV,block_pos,block_size);
            step_count++;
        }
    }
    printf("step : %d\n",step_count);
    cv::imshow("mv line",MV_line);
    cv::imshow("mv color",MV_color);
}


