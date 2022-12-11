#include "EBMA_lib.h"

static bool Check_search_boundry(cv::Point &search_pt,int M_width,int M_height)
{
    if(search_pt.x < M_width && search_pt.x >= 0 && search_pt.y < M_height && search_pt.y >= 0)
        return true;
    else
        return false;
}
void Diamond_search(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    cv::Point pre_mv_temp = mv_temp;
    mv_temp = cv::Point(0,0);
    bool not_first_step = false;
    if(min != 10000)
        not_first_step = true;
    for(int Sx = -1*step_size ; Sx <= 1*step_size ; Sx += step_size)
    {
        for(int Sy = -1*step_size ; Sy <= 1*step_size ; Sy += step_size)
        {
            // ignore search point of last direction
            if( (Sx != 0 && Sy != 0) || (Sx == (-pre_mv_temp.x) && Sy == (-pre_mv_temp.y))  )
                continue;
            // if not first diamand search , ignore center
            if((not_first_step == true) && (Sx == 0 && Sy == 0))
                continue;

            cv::Point search_v = cv::Point(Sx,Sy);
            cv::Point search_pt(now_pos.x+Sx,now_pos.y+Sy);
            cv::Point now_mv(search_pt.x - block_pos.x,search_pt.y - block_pos.y);

            n_search_point++;
            if(Check_search_boundry(search_pt,M_width,M_height))
            {
                double criterion = Cal_EOF(current_frame,ref_frame,now_mv,block_pos,block_size);
                if(criterion < min)
                {
                    min = criterion;
                    mv_temp = search_v;
                }
            }
            else
                continue;
        }
    }

    return;
}
void Square_search(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    mv_temp = cv::Point(0,0);
    bool not_first_step = false;
    if(min != 10000)
        not_first_step = true;
    for(int Sx = -1*step_size ; Sx <= 1*step_size ; Sx += step_size)
    {
        for(int Sy = -1*step_size ; Sy <= 1*step_size ; Sy += step_size)
        {
            cv::Point search_v = cv::Point(Sx,Sy);
            cv::Point search_pt(now_pos.x+Sx,now_pos.y+Sy);
            cv::Point now_mv(search_pt.x - block_pos.x,search_pt.y - block_pos.y);
            if((not_first_step == true) && (Sx == 0 && Sy == 0))
                continue;

            n_search_point++;
            if(Check_search_boundry(search_pt,M_width,M_height))
            {
                double criterion = Cal_EOF(current_frame,ref_frame,now_mv,block_pos,block_size);
                if(criterion < min  )
                {
                    min = criterion;
                    mv_temp = search_v;
                }
            }
        }
    }
}
void Fst_step_NTSS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    mv_temp = cv::Point(0,0);
    for(int Sx = -1*step_size ; Sx <= 1*step_size ; Sx += step_size)
    {
        for(int Sy = -1*step_size ; Sy <= 1*step_size ; Sy += step_size)
        {
            cv::Point search_v = cv::Point(Sx,Sy);
            cv::Point search_pt(now_pos.x+Sx,now_pos.y+Sy);
            cv::Point now_mv(search_pt.x - block_pos.x,search_pt.y - block_pos.y);

            n_search_point++;
            if(Check_search_boundry(search_pt,M_width,M_height))
            {
                double criterion = Cal_EOF(current_frame,ref_frame,now_mv,block_pos,block_size);
                if(criterion < min  )
                {
                    min = criterion;
                    mv_temp = search_v;
                }
            }
        }
    }
    for(int Sx = -1 ; Sx <= 1 ; Sx++ )
    {
        for(int Sy = -1 ; Sy <= 1 ; Sy++)
        {
            cv::Point search_v = cv::Point(Sx,Sy);
            cv::Point search_pt(now_pos.x+Sx,now_pos.y+Sy);
            cv::Point now_mv(search_pt.x - block_pos.x,search_pt.y - block_pos.y);
            if((Sx == 0 && Sy == 0))
                continue;

            n_search_point++;
            if(Check_search_boundry(search_pt,M_width,M_height))
            {
                double criterion = Cal_EOF(current_frame,ref_frame,now_mv,block_pos,block_size);
                if(criterion < min  )
                {
                    min = criterion;
                    mv_temp = search_v;
                }
            }
        }
    }
}
void Fast_Square_search(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Point &block_pos,cv::Point &now_pos,cv::Point &mv_temp,double &min,int &n_search_point,short block_size,short step_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    cv::Point pre_mv_temp = mv_temp;
    mv_temp = cv::Point(0,0);
    for(int Sx = -1*step_size ; Sx <= 1*step_size ; Sx += step_size)
    {
        for(int Sy = -1*step_size ; Sy <= 1*step_size ; Sy += step_size)
        {
            int position = std::abs(pre_mv_temp.x/step_size)+ std::abs(pre_mv_temp.y/step_size);
            if(position == 2) // corner
            {
                if( !(Sx == pre_mv_temp.x || Sy == pre_mv_temp.y))
                    continue;
            }
            else if(position == 1) //edge
            {
                if( !((Sx == (pre_mv_temp.x) && Sy == (pre_mv_temp.y)) || ((std::abs((Sx - pre_mv_temp.x)) + std::abs(Sy - pre_mv_temp.y)) == step_size )))
                    continue;
            }
            if(Sx == 0 && Sy == 0)
                continue;

            cv::Point search_v = cv::Point(Sx,Sy);
            cv::Point search_pt(now_pos.x+Sx,now_pos.y+Sy);
            cv::Point now_mv(search_pt.x - block_pos.x,search_pt.y - block_pos.y);

            n_search_point++;

            if(Check_search_boundry(search_pt,M_width,M_height))
            {
                double criterion = Cal_EOF(current_frame,ref_frame,now_mv,block_pos,block_size);
                if(criterion < min  )
                {
                    min = criterion;
                    mv_temp = search_v;
                }
            }
        }
    }
}
void BMA_4SS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    int candidate_cols = M_width/block_size;
    int candidate_rows = M_height/block_size;
    predict_frame = cv::Mat(current_frame.size(),current_frame.type());
    cv::Mat MV_line = cv::Mat::zeros(current_frame.size(),current_frame.type());
    cv::Mat MV_color = cv::Mat::zeros(current_frame.size(),CV_8UC3);
    short max_step = std::ceil(search_size/2);
    int max_search_pt = 0;
    int min_search_pt = 1000;

    for(int j = 0; j < candidate_rows;j++)
    {
        for(int i = 0;i < candidate_cols;i++)
        {
            cv::Point MV(0,0);

            cv::Point block_pos(i*block_size,j*block_size); // the left top corner position of the candidate block
            cv::Point now_pos = block_pos;                  // the moving search point with square or diamand search
            cv::Point mv_temp(-1,-1);                       // the temp best search point

            short step_size = 2;
            bool search_one_block = true;
            double min = 10000;
            int step = 1;
            int n_search_point = 0 ;
            while(search_one_block)
            {
                if(step == 1)
                {
                    Square_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,step_size);
                    if(mv_temp.x == 0 && mv_temp.y == 0)
                        step = max_step;
                    else
                        step++;
                }
                else if(step == max_step)
                {
                    step_size = 1;
                    Square_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,step_size);
                    search_one_block = false;

                }
                else
                {
                    Fast_Square_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,step_size);
                    if(mv_temp.x == 0 && mv_temp.y == 0)
                        step = max_step;
                    else
                        step++;
                }

                MV.x += mv_temp.x;
                MV.y += mv_temp.y;
                now_pos.x += mv_temp.x;
                now_pos.y += mv_temp.y;

            }
            if(n_search_point > max_search_pt)
                max_search_pt = n_search_point;
            if(n_search_point < min_search_pt)
                min_search_pt = n_search_point;
            //printf("step : %d\n",step);
            cv::Point global_MV(block_pos.x+MV.x,block_pos.y+MV.y);
            cv::line(MV_line,block_pos,global_MV,cv::Scalar(255));
            Set_MV_color(MV_color,MV,block_pos,block_size);
            Predict_ref_frame(predict_frame,ref_frame,MV,block_pos,block_size);
        }
    }
    printf("max search pt n : %d \n",max_search_pt);
    printf("min search pt n : %d \n",min_search_pt);
    cv::imshow("mv line",MV_line);
    cv::imshow("mv color",MV_color);
}
void BMA_NTSS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    int candidate_cols = M_width/block_size;
    int candidate_rows = M_height/block_size;
    predict_frame = cv::Mat(current_frame.size(),current_frame.type());
    cv::Mat MV_line = cv::Mat::zeros(current_frame.size(),current_frame.type());
    cv::Mat MV_color = cv::Mat::zeros(current_frame.size(),CV_8UC3);
    short step_size = std::ceil(search_size/2);
    int max_search_pt = 0;
    int min_search_pt = 1000;

    for(int i = 0; i < candidate_rows;i++)
    {
        for(int j = 0;j < candidate_cols;j++)
        {
            cv::Point MV(0,0);

            cv::Point block_pos(j*block_size,i*block_size); // the left top corner position of the candidate block
            cv::Point now_pos = block_pos;                  // the moving search point with square or diamand search
            cv::Point mv_temp(-1,-1);                       // the temp best search point

            short temp_step_size = step_size;
            bool search_one_block = true;
            double min = 10000;
            int step = 1;
            int n_search_point = 0;
            while(search_one_block)
            {
                if(step == 1)
                {
                    Fst_step_NTSS(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,temp_step_size);
                    if(mv_temp.x == 0 && mv_temp.y == 0)
                        search_one_block = false;
                    else if( std::abs(mv_temp.x) == 1 || std::abs(mv_temp.y) == 1)
                    {
                        temp_step_size = 1;
                        step = 2;
                    }
                    else
                    {
                        temp_step_size = std::ceil((double)temp_step_size/2);
                        step = 3;
                    }
                }
                else if(step == 2 )
                {
                    Fast_Square_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,temp_step_size);
                    search_one_block = false;
                }
                else if(step == 3)
                {
                    Square_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,temp_step_size);
                    if(temp_step_size == 1 || (mv_temp.x == 0 && mv_temp.y == 0))
                        search_one_block = false;
                    else
                        temp_step_size = std::ceil((double)temp_step_size/2);

                }

                MV.x += mv_temp.x;
                MV.y += mv_temp.y;
                now_pos.x += mv_temp.x;
                now_pos.y += mv_temp.y;

            }
            if(n_search_point > max_search_pt)
                max_search_pt = n_search_point;
            if(n_search_point < min_search_pt)
                min_search_pt = n_search_point;
            //printf("search pt n : %d \n",n_search_point);
            //printf("step : %d \n",step);

            cv::Point global_MV(block_pos.x+MV.x,block_pos.y+MV.y);
            cv::line(MV_line,block_pos,global_MV,cv::Scalar(255));
            Set_MV_color(MV_color,MV,block_pos,block_size);
            Predict_ref_frame(predict_frame,ref_frame,MV,block_pos,block_size);
        }
    }
    printf("max search pt n : %d \n",max_search_pt);
    printf("min search pt n : %d \n",min_search_pt);
    cv::imshow("mv line",MV_line);
    cv::imshow("mv color",MV_color);
}
void BMA_TSS(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    int candidate_cols = M_width/block_size;
    int candidate_rows = M_height/block_size;
    predict_frame = cv::Mat(current_frame.size(),current_frame.type());
    cv::Mat MV_line = cv::Mat::zeros(current_frame.size(),current_frame.type());
    cv::Mat MV_color = cv::Mat::zeros(current_frame.size(),CV_8UC3);
    short step_size = std::ceil((double)search_size/2);
    printf("step size : %d\n",step_size);
    int max_search_pt = 0;
    int min_search_pt = 1000;

    for(int j = 0; j < candidate_rows;j++)
    {
        for(int i = 0;i < candidate_cols;i++)
        {
            cv::Point MV(0,0);

            cv::Point block_pos(i*block_size,j*block_size); // the left top corner position of the candidate block
            cv::Point now_pos = block_pos;                  // the moving search point with square or diamand search
            cv::Point mv_temp(-1,-1);                       // the temp best search point

            short temp_step_size = step_size;
            bool search_one_block = true;
            double min = 10000;
            int n_search_point = 0 ;

            while(search_one_block)
            {
                if(temp_step_size == 1)
                    search_one_block = false;

                Square_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,temp_step_size);
                temp_step_size = std::ceil((double)temp_step_size/2);

                MV.x += mv_temp.x;
                MV.y += mv_temp.y;
                now_pos.x += mv_temp.x;
                now_pos.y += mv_temp.y;
                //printf("step size : %d\n",step_size);
            }
            if(n_search_point > max_search_pt)
                max_search_pt = n_search_point;
            if(n_search_point < min_search_pt)
                min_search_pt = n_search_point;
            //printf("step : %d\n",step);
            cv::Point global_MV(block_pos.x+MV.x,block_pos.y+MV.y);
            cv::line(MV_line,block_pos,global_MV,cv::Scalar(255));
            Set_MV_color(MV_color,MV,block_pos,block_size);
            Predict_ref_frame(predict_frame,ref_frame,MV,block_pos,block_size);
        }
    }
    printf("max search pt n : %d \n",max_search_pt);
    printf("min search pt n : %d \n",min_search_pt);
    cv::imshow("mv line",MV_line);
    cv::imshow("mv color",MV_color);
}
void BMA_2Dlog(cv::Mat &current_frame,cv::Mat &ref_frame,cv::Mat &predict_frame,short search_size,short block_size)
{
    int M_width = current_frame.cols;
    int M_height = current_frame.rows;
    int candidate_cols = M_width/block_size;
    int candidate_rows = M_height/block_size;
    predict_frame = cv::Mat(current_frame.size(),current_frame.type());
    cv::Mat MV_line = cv::Mat::zeros(current_frame.size(),current_frame.type());
    cv::Mat MV_color = cv::Mat::zeros(current_frame.size(),CV_8UC3);
    int n = std::floor(std::log2(search_size));
    int temp_n = std::pow(2,n-1);
    short step_size = std::max(2,temp_n);
    printf("step size : %d\n",step_size);
    int max_search_pt = 0;
    int min_search_pt = 1000;

    for(int j = 0; j < candidate_rows;j++)
    {
        for(int i = 0;i < candidate_cols;i++)
        {
            cv::Point MV(0,0);

            cv::Point block_pos(i*block_size,j*block_size); // the left top corner position of the candidate block
            cv::Point now_pos = block_pos;                  // the moving search point with square or diamand search
            cv::Point mv_temp(-1,-1);                       // the temp best search point

            short temp_step_size = step_size;
            bool search_one_block = true;
            double min = 10000;
            int n_search_point = 0 ;
            int step = 1;
            while(search_one_block)
            {
                if(temp_step_size == 1)
                {
                    Square_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,temp_step_size);
                    search_one_block = false;
                }
                else
                    Diamond_search(current_frame,ref_frame,block_pos,now_pos,mv_temp,min,n_search_point,block_size,temp_step_size);

                MV.x += mv_temp.x;
                MV.y += mv_temp.y;
                now_pos.x += mv_temp.x;
                now_pos.y += mv_temp.y;
                if( (mv_temp.x == 0 && mv_temp.y == 0) || std::abs(MV.x) == search_size-1 || std::abs(MV.y) == search_size-1)
                    temp_step_size = std::ceil((double)temp_step_size/2);

                //if(n_search_point == 29)
                //{
                //    printf("step : %d \t",step);
                //    //printf("mv ( %d , %d )\n",MV.x,MV.y);
                //}
                step++;
            }

            cv::Point global_MV(block_pos.x+MV.x,block_pos.y+MV.y);
            cv::line(MV_line,block_pos,global_MV,cv::Scalar(255));
            Set_MV_color(MV_color,MV,block_pos,block_size);
            Predict_ref_frame(predict_frame,ref_frame,MV,block_pos,block_size);
            if(n_search_point > max_search_pt)
                max_search_pt = n_search_point;
            if(n_search_point < min_search_pt)
                min_search_pt = n_search_point;
        }
    }
    printf("max search pt n : %d \n",max_search_pt);
    printf("min search pt n : %d \n",min_search_pt);
    cv::imshow("mv line",MV_line);
    cv::imshow("mv color",MV_color);
}

