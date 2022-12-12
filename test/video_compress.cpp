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
    cv::Mat error_float_img;
    cv::Mat dct_error_img;
    cv::Mat idct_error_img;
    cv::Mat limited_dct_error_img;
    cv::Mat quantized_dct_error_img;
    cv::Mat limited_idct_error_img;
    cv::Mat quantized_idct_img;
    cv::Mat show_dct_img;
    cv::Mat show_idct_img;
    cv::Mat show_limited_idct_img;
    cv::Mat show_limited_dct_img;
    cv::Mat show_quantized_idct_img;
    cv::Mat show_quantized_dct_img;
    cv::Mat show_error_img;
    cv::Mat show_reconstruct_img;
    cv::Mat reconstruct_img;


    Get_chrominance(cur_img,L_cur_frame);
    Get_chrominance(ref_img,L_ref_frame);

    //EBMA(L_cur_frame,L_ref_frame,predict_frame,32,16);
    BMA_NTSS(L_cur_frame,L_ref_frame,predict_frame,32,16);
    double mse = Cal_MSE(predict_frame,L_ref_frame);
    double PSNR = Cal_PSNR(mse) ;

    //predict_frame.clone().convertTo(error_img,CV_32F,1/255.0);
    //error_img.convertTo(show_error_img,CV_8UC1,255);
    //Get_error_image(L_cur_frame, predict_frame, error_img);
    //Add_image(predict_frame,error_img,reconstruct_img);
    //error_img.convertTo(show_error_img,CV_8UC1,255);
    //reconstruct_img.convertTo(show_reconstruct_img,CV_8U,255);
    cv::subtract(L_cur_frame,predict_frame,error_img);

    //predict_frame.convertTo(predict_frame, CV_32F,1/255.0);
    error_img.convertTo(error_float_img, CV_32F);
    //DCT_transform(predict_frame, dct_error_img);
    DCT_transform(error_float_img, dct_error_img);
    zigzag_limit(dct_error_img, limited_dct_error_img);
    Quantizer(limited_dct_error_img,quantized_dct_error_img);

    //IDCT_transform(limited_dct_error_img, limited_idct_error_img);
    //IDCT_transform(dct_error_img, idct_error_img);
    IDCT_transform(quantized_dct_error_img, quantized_idct_img);


    //error_img.convertTo(error_img, CV_32F,1/255.0);
    //cv::dct(error_img, dct_error_img);
    //DCT_transform(error_img, dct_error_img);
    //IDCT_transform(dct_error_img, idct_error_img);

    //idct_error_img.convertTo(show_idct_img, CV_8UC1);

    dct_error_img.convertTo(show_dct_img,CV_8UC1);
    limited_dct_error_img.convertTo(show_limited_dct_img,CV_8UC1);
    quantized_dct_error_img.convertTo(show_quantized_dct_img, CV_8UC1);
    quantized_idct_img.convertTo(show_quantized_idct_img, CV_8UC1);

    //cv::add(predict_frame, error_img, reconstruct_img);
    cv::add(predict_frame, show_quantized_idct_img, reconstruct_img);
    printf("origin mse %3f\n",mse);
    printf("origin PSNR %3f\n", PSNR);
    double compressed_mse = Cal_MSE(reconstruct_img,L_ref_frame);
    double compressed_PSNR = Cal_PSNR(compressed_mse) ;
    printf("compressed mse %3f\n",compressed_mse);
    printf("compressed PSNR %3f\n", compressed_PSNR);

    cv::imshow("predict_frame",predict_frame);
    cv::imshow("current frame",L_cur_frame);
    cv::imshow("ref frame",L_ref_frame);
    cv::imshow("error frame",error_img);
    cv::imshow("show dct error frame",show_dct_img);
    cv::imshow("show limited dct error frame",show_limited_dct_img);
    //cv::imshow("show idct error frame",show_idct_img);
    //cv::imshow("show limited idct error frame",show_limited_idct_img);
    cv::imshow("show quantized limited idct error frame",show_quantized_idct_img);
    cv::imshow("show quantized limited dct error frame",show_quantized_dct_img);
    cv::imshow("show reconstruction frame",reconstruct_img);
    cv::waitKey(0);
    return 0;


}
