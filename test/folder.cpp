#include "EBMA_lib.h"
#include "fast_BMA_lib.h"
#include "video_compress_lib.h"
#include <string>

int main(int argc,char** argv)
{

    std::string images_path = std::string(argv[1]);
    std::vector<std::string> filenames_in_folder;
    cv::String images_path_cv = images_path;
    std::vector<cv::String> filenames_in_folder_cv;
    //取得照片
    cv::glob(images_path_cv, filenames_in_folder_cv);
    for (auto &i : filenames_in_folder_cv)
        filenames_in_folder.push_back(i);


    std::vector<std::string> png_filenames_path;
    std::vector<std::string> png_filenames;
    std::vector<std::string> png_filenames_without_ext;

    //確認檔名
    for(auto &i : filenames_in_folder)
    {
            int pos_filename = 0;
            if ((1 + i.find_last_of("\\")) < i.length()) pos_filename = 1 + i.find_last_of("\\");
            if ((1 + i.find_last_of("/")) < i.length()) pos_filename = std::max(pos_filename, 1 + (int)i.find_last_of("/"));

            std::string const filename = i.substr(pos_filename);
            std::string const ext = i.substr(i.find_last_of(".") + 1);
            std::string const filename_without_ext = filename.substr(0, filename.find_last_of("."));

            if (ext == "png")
            {
                png_filenames_without_ext.push_back(filename_without_ext);
                png_filenames.push_back(filename);
                png_filenames_path.push_back(i);
            }
    }

    std::sort(png_filenames.begin(), png_filenames.end());
    std::sort(png_filenames_path.begin(), png_filenames_path.end());

    if (png_filenames.size() == 0) {
        std::cout << "Error: Image files not found by path: " << images_path << std::endl;
        return 0;
    }
    //std::cout << "find " << png_filenames.size() << "of plate in this folder" << std::endl;

    std::vector<cv::Mat> images_array;
    std::vector<cv::Mat> images_L_array;
    for(size_t i  = 0; i < png_filenames.size(); i ++)
    {
        cv::Mat src = imread(png_filenames_path[i],cv::IMREAD_COLOR);
        images_array.push_back(src);
    }
    std::vector<cv::Mat> Lab_vector;
    for(size_t i = 0; i < images_array.size();i++)
    {
        cv::Mat Lab;
        Get_chrominance(images_array.at(i),Lab);
        images_L_array.push_back(Lab.clone());
    }
    printf("L array size : %d\n",(int)images_L_array.size());
    clock_t start = clock();
    for(size_t i = 0 ; i < images_L_array.size()-1;i++)
    {
        cv::Mat predict;
        cv::Mat current_frame = images_L_array.at(i).clone();
        cv::Mat ref_frame = images_L_array.at(i+1).clone();
        //EBMA(current_frame,ref_frame,predict,8,4);
        //BMA_4SS(current_frame,ref_frame,predict,8,4);
        //BMA_TSS(current_frame,ref_frame,predict,8,4);
        //BMA_2Dlog(current_frame,ref_frame,predict,8,4);
        BMA_NTSS(current_frame,ref_frame,predict,8,4);
        double avg_mse = Cal_MSE(predict,ref_frame);
        printf("avg mse : %3f\n",avg_mse);
    }
    clock_t end = clock();
    float total_time = (float)((end - start) / CLOCKS_PER_SEC);
    printf("cost time per frame : %0.8f sec \n", total_time);
    printf("avg cost time per frame : %0.8f sec \n", total_time/9.0);
    //cv::imshow("current frame",images_L_array.at(0));
    //cv::imshow("current frame",current_frame);
    //cv::imshow("ref frame",ref_frame);
    //EBMA(current_frame,ref_frame,predict,7,4);
    //BMA_2Dlog(current_frame,ref_frame,predict,6,4);
    //double avg_mse = Cal_MSE(current_frame,ref_frame);
    //printf("avg mse : %3f\n",avg_mse);
    //cv::imshow("predict frame",predict);
    //cv::waitKey(0);

    cv::destroyAllWindows();

    return 0 ;


}

