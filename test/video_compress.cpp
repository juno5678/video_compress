#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string.h>
#include <iostream>
#include "EBMA_lib.h"
#include "fast_BMA_lib.h"
#include "video_compress_lib.h"


int main(int argc,char **argv)
{
    if(argc != 3)
    {
        printf("you have to type the two picture path\n");
        return 0;
    }
    std::string current_frame_path = argv[1];
    //printf("current frame path : %s\n",current_frame_path.c_str());
    std::string ref_frame_path = argv[2];
    //printf("ref frame path : %s\n",ref_frame_path.c_str());
    cv::Mat cur_frame = cv::imread(current_frame_path);

    if(cur_frame.empty())
    {
        printf("%s didm't have picture\n",current_frame_path.c_str());
        return 0;
    }
    cv::Mat cur_img = cur_frame.clone();

    cv::Mat ref_frame = cv::imread(ref_frame_path);
    if(ref_frame.empty())
    {
        printf("%s didm't have picture\n",ref_frame_path.c_str());
        return 0;
    }
    cv::Mat ref_img = ref_frame.clone();

    cv::Mat predict_frame;
    cv::Mat diff_frame;
    cv::Mat L_cur_frame,L_ref_frame;
    cv::Mat error_img;
    cv::Mat dct_error_img;
    cv::Mat idct_error_img;
    cv::Mat show_dct_img;
    cv::Mat show_idct_img;

    Get_chrominance(cur_img,L_cur_frame);
    Get_chrominance(ref_img,L_ref_frame);

    BMA_NTSS(L_cur_frame,L_ref_frame,predict_frame,8,4);
    double mse = Cal_MSE(predict_frame,L_ref_frame);
    double PSNR = Cal_PSNR(mse) ;
    Get_error_image(L_ref_frame, predict_frame, error_img);
    predict_frame.convertTo(predict_frame, CV_32F);
    //cv::dct(error_img, dct_error_img);
    DCT_transform(predict_frame, dct_error_img);
    dct_error_img.convertTo(show_dct_img,CV_8UC1);
    IDCT_transform(dct_error_img, idct_error_img);
    idct_error_img.convertTo(idct_error_img, CV_8UC1);


    printf("mse %3f\n",mse);
    printf("PSNR %3f\n", PSNR);

    cv::imshow("predict_frame",predict_frame);
    cv::imshow("current frame",L_cur_frame);
    cv::imshow("ref frame",L_ref_frame);
    cv::imshow("error frame",error_img);
    cv::imshow("show dct error frame",show_dct_img);
    cv::waitKey(0);
    return 0;


}
