#include "video_compress_lib.h"
#include "opencv2/core.hpp"
#include <cmath>

void Get_chrominance(cv::Mat &origin_img,cv::Mat &chrom_img)
{
    std::vector<cv::Mat> spl_cvtColor_img;
    cv::Mat cvtColor_img;
    cv::cvtColor(origin_img,cvtColor_img,cv::COLOR_BGR2Lab);
    cv::split(cvtColor_img,spl_cvtColor_img);
    chrom_img = spl_cvtColor_img.at(0);

}
double Cal_MSE(const cv::Mat &predict_frame,const cv::Mat &target_frame)
{
    double sum = 0;
    double MSE;

    for(int i = 0 ; i < predict_frame.rows;i++)
    {
        for(int j = 0 ; j < predict_frame.cols;j++)
        {
            int predict_I = predict_frame.at<uchar>(i,j);
            int target_I = target_frame.at<uchar>(i,j);
            MSE = std::round(std::sqrt(std::pow(predict_I - target_I,2))*1000)/1000;
            sum +=MSE;

        }
    }
    return sum/(predict_frame.rows*predict_frame.cols);
}
double Cal_PSNR(double mse, int max_i)
{
    return 10 * std::log10((max_i*max_i)/mse);
}


void Get_error_image(cv::Mat &origin_img, cv::Mat &compare_img, cv::Mat &error_img)
{
    error_img = cv::Mat::zeros(origin_img.size(),origin_img.type());
    for(int i = 0 ; i < origin_img.rows; i++)
    {
        for(int j = 0 ; j < origin_img.cols ; j++)
        {
            error_img.at<uchar>(i,j) = std::abs( origin_img.at<uchar>(i,j) - compare_img.at<uchar>(i,j) );
        }
    }
}
void DCT_transform(cv::Mat &src, cv::Mat &output, int size)
{
    //cv::Mat resized_src;
    //cv::resize(src,resized_src,cv::Size(64,64));
    //size = 64;
    output = cv::Mat::zeros(src.size(),src.type());
    float Cu, Cv ;

    int block_u = 1;
    for(int u = 0 ; u < src.rows; u = u+size)
    {
        int block_v = 1;
        for(int v = 0 ; v < src.cols ; v = v+size)
        {
            float temp = 0.0;
            for(int m = 0 ; m < size ; m++)
            {
                for(int n = 0 ; n < size ; n++)
                {
                    if(u+m >= size*block_u || v+n >= size*block_v)
                        continue;
                    temp += src.at<float>(u+m,v+n) * std::cos(((2*m+1)*u*M_PI)/(2*size)) * std::cos(((2*n+1)*v*M_PI)/(2*(float)size));
                    //printf(" (%d, %d) : %3f \t",m,n,src.at<float>(u,v));
                }
               // printf("\n");
            }
            if(u == 0 && v == 0)
                Cu = Cv = std::sqrt(1/(float)size);
            else
                Cu = Cv = std::sqrt(2/(float)size);
            output.at<float>(u,v) = Cu * Cv * temp;
            //printf("temp : %3f\t after : %3f\t ",temp, output.at<float>(u,v));
            //printf("%f\t",output.at<float>(u,v));
            //printf("%f\t",src.at<float>(u,v));
            block_v++;
        }
        block_u++;
        //printf("\n");
    }

    cv::imshow("dct output", output);

}
void IDCT_transform(cv::Mat &src, cv::Mat &output, int size)
{
    output = cv::Mat::zeros(src.size(),src.type());
    float Cu, Cv ;

    int block_u = 1;
    for(int u = 0 ; u < src.rows; u = u+size)
    {
        int block_v = 1;
        for(int v = 0 ; v < src.cols ; v = v+size)
        {
            float temp = 0.0;
            if(u == 0 && v == 0)
                Cu = Cv = std::sqrt(1/(float)size);
            else
                Cu = Cv = std::sqrt(2/(float)size);
            for(int m = 0 ; m < size ; m++)
            {
                for(int n = 0 ; n < size ; n++)
                {
                    if(u+m >= size*block_u || v+n >= size*block_v)
                        continue;
                    temp += Cu * Cv * src.at<float>(u+m,v+n) * std::cos(((2*m+1)*u*M_PI)/(2*size)) * std::cos(((2*n+1)*v*M_PI)/(2*(float)size));
                    //printf(" (%d, %d) : %3f \t",m,n,src.at<float>(u,v));
                }
               // printf("\n");
            }
            output.at<float>(u,v) = temp;
            //printf("temp : %3f\t after : %3f\t ",temp, output.at<float>(u,v));
            //printf("%f\t",output.at<float>(u,v));
            //printf("%f\t",src.at<float>(u,v));
            block_v++;
        }
        //printf("\n");
        block_u++;
    }

    cv::imshow("idct output", output);

}
