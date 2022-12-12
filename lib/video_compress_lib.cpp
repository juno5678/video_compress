#include "video_compress_lib.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
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
    cv::Mat src_float;
    cv::Mat src_uint;
    cv::Mat compare_img_float;
    origin_img.clone().convertTo(src_float,CV_32F,1/255.0);
    src_float.convertTo(src_uint,CV_8UC1,255);
    compare_img.clone().convertTo(compare_img_float,CV_32F,1/255.0);
    error_img = cv::Mat::zeros(origin_img.size(),CV_32F);
    for(int i = 0 ; i < origin_img.rows; i++)
    {
        for(int j = 0 ; j < origin_img.cols ; j++)
        {
            error_img.at<float>(i,j) =  origin_img.at<float>(i,j) - compare_img.at<float>(i,j) ;
        }
    }
    cv::imshow("origin img",src_uint);
}
void Add_image(cv::Mat &origin_img, cv::Mat &compare_img, cv::Mat &output_img)
{
    cv::Mat src_float;
    cv::Mat compare_img_float;
    origin_img.clone().convertTo(src_float,CV_32F,1/255.0);
    compare_img.clone().convertTo(compare_img_float,CV_32F,1/255.0);
    output_img = cv::Mat::zeros(origin_img.size(),CV_32F);
    for(int i = 0 ; i < origin_img.rows; i++)
    {
        for(int j = 0 ; j < origin_img.cols ; j++)
        {
            output_img.at<float>(i,j) =  origin_img.at<float>(i,j) - compare_img.at<float>(i,j) ;
        }
    }
}

void DCT2D(cv::Mat &src, cv::Mat &output, int size)
{

    output = cv::Mat::zeros(src.size(),src.type());
    float Cu, Cv ;
    for(int u = 0 ; u < size ; u++)
    {
        for(int v = 0; v < size ; v++)
        {
            float temp = 0.0;
            for(int m = 0 ; m < size ; m++)
            {
                for(int n = 0 ; n < size ; n++)
                {
                    temp += src.at<float>(m,n) * std::cos(((2*m+1)*u*M_PI)/(2*size)) * std::cos(((2*n+1)*v*M_PI)/(2*(float)size));
                    //printf(" (%d, %d) : %3f \t",m,n,src.at<float>(u,v));
                }
               // printf("\n");
            }
            if(u == 0 && v == 0)
                Cu = Cv = std::sqrt(1/(float)size);
            else
                Cu = Cv = std::sqrt(2/(float)size);
            output.at<float>(u,v) = Cu * Cv * temp;
        }
    }
    //cv::imshow("block", src);
    //cv::imshow("dct block", output);
    //cv::waitKey(0);
}

void IDCT2D(cv::Mat &src, cv::Mat &output, int size)
{

    output = cv::Mat::zeros(src.size(),src.type());
    float Cu, Cv ;
    for(int m = 0 ; m < size ; m++)
    {
        for(int n = 0; n < size ; n++)
        {
            float temp = 0.0;
            for(int u = 0 ; u < size ; u++)
            {
                for(int v = 0 ; v < size ; v++)
                {
                    if(u == 0 && v == 0)
                        Cu = Cv = std::sqrt(1/(float)size);
                    else
                        Cu = Cv = std::sqrt(2/(float)size);
                    temp += Cu * Cv * src.at<float>(u,v) * std::cos(((2*m+1)*u*M_PI)/(2*size)) * std::cos(((2*n+1)*v*M_PI)/(2*(float)size));
                    //printf(" (%d, %d) : %3f \t",m,n,src.at<float>(u,v));
                }
               // printf("\n");
            }
            output.at<float>(m,n) = temp;
        }
    }
    //cv::imshow("block", src);
    //cv::imshow("dct block", output);
    //cv::waitKey(0);
}
void DCT_transform(cv::Mat &src, cv::Mat &output, int size)
{
    output = cv::Mat::zeros(src.size(),src.type());
    cv::Mat block;
    cv::Mat dct_block;
    cv::Mat output_block;

    for(int i = 0 ; i < src.rows; i = i+size)
    {
        for(int j = 0 ; j < src.cols ; j = j+size)
        {
            block = src(cv::Rect(j,i,size,size)).clone();
            output_block = output(cv::Rect(j,i,size,size));
            DCT2D(block, dct_block, size);
            dct_block.copyTo(output_block);
        }
    }

    //cv::imshow("dct output", output);

}
void IDCT_transform(cv::Mat &src, cv::Mat &output, int size)
{
    output = cv::Mat::zeros(src.size(),src.type());
    cv::Mat block;
    cv::Mat idct_block;
    cv::Mat output_block;

    for(int i = 0 ; i < src.rows; i = i+size)
    {
        for(int j = 0 ; j < src.cols ; j = j+size)
        {
            block = src(cv::Rect(j,i,size,size)).clone();
            output_block = output(cv::Rect(j,i,size,size));
            IDCT2D(block, idct_block, size);
            idct_block.copyTo(output_block);
        }
    }

    //cv::imshow("idct output", output);

}
void zigzag_limit(cv::Mat &src, cv::Mat &output, int size,  int keep_num)
{

    output = cv::Mat::zeros(src.size(),src.type());
    cv::Mat block;
    cv::Mat limited_block;
    cv::Mat output_block;

    for(int i = 0 ; i < src.rows; i = i+size)
    {
        for(int j = 0 ; j < src.cols ; j = j+size)
        {
            block = src(cv::Rect(j,i,size,size)).clone();
            output_block = output(cv::Rect(j,i,size,size));
            zigzag_remove(block, limited_block, size, 4);
            limited_block.copyTo(output_block);
        }
    }

    //cv::imshow("zigzag dct input", src);
    //cv::imshow("zigzag limited dct output", output);
}
void zigzag_remove(cv::Mat &src, cv::Mat &output,int size, int keep_num)
{
    output = cv::Mat::zeros(src.size(),src.type());
    int i = 0, j = 0;

    int step =-1;
    int diagonal_step = 1;
    int diagonal_count = 0 ;
    bool over_half = false;
    bool run = true;
    int total_count = 0;

    while(run)
    {

        switch(step % 4)
        {
            case 0:
                if(over_half == false)
                    j++;
                else
                    i++;
                step++;
                break;
            case 1:
                i = i + 1;
                j = j - 1;
                diagonal_count++;
                if(diagonal_count == diagonal_step)
                {
                    diagonal_count = 0;
                    step++;
                    if(over_half == false)
                        diagonal_step++;
                    else
                        diagonal_step--;
                }
                break;
            case 2:
                if(over_half == false)
                    i++;
                else
                    j++;
                step++;
                break;
            case 3:
                i = i - 1;
                j = j + 1;
                diagonal_count++;
                if(diagonal_count == diagonal_step)
                {
                    diagonal_count = 0;
                    step++;
                    if(over_half == false)
                        diagonal_step++;
                    else
                        diagonal_step--;
                }
                break;
            default:
                step++;
        }


        if(diagonal_step == size)
        {
            diagonal_step -=1;
            over_half = true;
        }

        if(total_count < keep_num)
            output.at<float>(i,j) = src.at<float>(i,j);

        //if(total_count == 0)
        //    printf("%3f \t",src.at<float>(i,j));
        total_count++;

        if((i == size-1 && j == size-1 ) || total_count == keep_num )
            run = false;

        //printf("step : %d, (%d, %d)\t",step,i,j);
        //printf("diagonal_step : %d\t diagonal_count : %d\n",diagonal_step,diagonal_count);

    }
    //printf("\n");

    //cv::imshow("block", src);
    //cv::imshow("limited block", output);
    //cv::waitKey(0);
}

void Quantizer(cv::Mat &src, cv::Mat &output,int size)
{
    output = cv::Mat::zeros(src.size(),src.type());
    int coff_num = 4;
    std::vector<float> max(coff_num,0.0);
    std::vector<float> min(coff_num,100.0);
    std::vector<float> q(coff_num,0);

    cv::Mat block;
    cv::Mat output_block;
    std::vector<float> zigzag_1Darray(size*size,0);
    std::vector<float> quantized_zigzag_1Darray(size*size,0);

    // find min and max
    for(int i = 0 ; i < src.rows; i = i+size)
    {
        for(int j = 0 ; j < src.cols ; j = j+size)
        {
            block = src(cv::Rect(j,i,size,size)).clone();
            zigzag_scan(block,zigzag_1Darray,size,coff_num,true);
            for(int k = 0 ; k < coff_num ; k++)
            {
                if(zigzag_1Darray.at(k) > max.at(k))
                    max.at(k) = zigzag_1Darray.at(k);
                else if(zigzag_1Darray.at(k) < min.at(k))
                    min.at(k) = zigzag_1Darray.at(k);
            }
        }
    }

    // calculate q
    for(size_t i = 0 ; i < q.size();i++)
    {
        if(i == 0)
            q.at(i) = (max.at(i) - min.at(i))/16;
        else
            q.at(i) = (max.at(i) - min.at(i))/4;
    }

    // quantize and inverse quantize
    for(int i = 0 ; i < src.rows; i = i+size)
    {
        for(int j = 0 ; j < src.cols ; j = j+size)
        {
            block = src(cv::Rect(j,i,size,size)).clone();
            output_block = output(cv::Rect(j,i,size,size));
            zigzag_scan(block,zigzag_1Darray,size,coff_num,true);
            for(int k = 0 ; k < coff_num ; k++)
            {
                quantized_zigzag_1Darray.at(k) = std::floor((zigzag_1Darray.at(k) - min.at(k))/q.at(k)) * q.at(k) + (q.at(k)/2.0) + min.at(k);
            }
            zigzag_scan(output_block, quantized_zigzag_1Darray, size,coff_num, false);
        }
    }

}

void zigzag_scan(cv::Mat &io_mat,std::vector<float> &zigzag_1Darray, int size, int get_num , bool read_or_write)
{
    int i = 0, j = 0;

    int step =-1;
    int diagonal_step = 1;
    int diagonal_count = 0 ;
    bool over_half = false;
    bool run = true;
    int total_count = 0;

    while(run)
    {

        switch(step % 4)
        {
            case 0:
                if(over_half == false)
                    j++;
                else
                    i++;
                step++;
                break;
            case 1:
                i = i + 1;
                j = j - 1;
                diagonal_count++;
                if(diagonal_count == diagonal_step)
                {
                    diagonal_count = 0;
                    step++;
                    if(over_half == false)
                        diagonal_step++;
                    else
                        diagonal_step--;
                }
                break;
            case 2:
                if(over_half == false)
                    i++;
                else
                    j++;
                step++;
                break;
            case 3:
                i = i - 1;
                j = j + 1;
                diagonal_count++;
                if(diagonal_count == diagonal_step)
                {
                    diagonal_count = 0;
                    step++;
                    if(over_half == false)
                        diagonal_step++;
                    else
                        diagonal_step--;
                }
                break;
            default:
                step++;
        }


        if(diagonal_step == size)
        {
            diagonal_step -=1;
            over_half = true;
        }

        if(total_count < get_num)
        {
            if(read_or_write == true)
                zigzag_1Darray.at(total_count) = io_mat.at<float>(i,j);
            else
                io_mat.at<float>(i,j) = zigzag_1Darray.at(total_count);
        }

        total_count++;

        if((i == size-1 && j == size-1 ) || total_count == get_num )
            run = false;

        //printf("step : %d, (%d, %d)\t",step,i,j);
        //printf("diagonal_step : %d\t diagonal_count : %d\n",diagonal_step,diagonal_count);

    }
    //printf("\n");

    //cv::imshow("block", src);
    //cv::imshow("limited block", output);
    //cv::waitKey(0);
}
void reconstruction(cv::Mat &predict_img, cv::Mat &compressed_error_image, cv::Mat &output)
{
    output = cv::Mat::zeros(predict_img.size(),predict_img.type());
    for(int i = 0 ; i < predict_img.rows; i++)
    {
        for(int j = 0 ; j < predict_img.cols ; j++)
        {
            output.at<uchar>(i,j) = predict_img.at<uchar>(i,j) + compressed_error_image.at<uchar>(i,j);
        }
    }
}
