#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <queue>
#include <omp.h>
#include <signal.h>
#include <chrono>
#include <iomanip>
#include "openni2_cv_grabber.h"

using namespace std;
using namespace cv;

bool SIGINT_RAISED = false;

void sigint_handler(int s){
    cout << "Wait until frames are saved.." << endl;
    SIGINT_RAISED = true;
}

int main(int argc, char **argv){
    signal(SIGINT, sigint_handler);

    Openni2_grabber *grabber = new Openni2_grabber();
    grabber->init_depth_stream();
    grabber->init_rgb_stream();

    Mat depth_frame, rgb_frame;
    queue<Mat*> color_buff, depth_buff;
    int counter_depth = -1, counter_color = -1;
    bool go = true;
    
    #pragma omp parallel shared(go, color_buff, depth_buff), num_threads(8)
    {
        while(go){
            #pragma omp sections nowait
            {
                #pragma omp section
                {
                    if(!SIGINT_RAISED){
                        grabber->retrieve(depth_frame, Openni2_grabber::DEPTH_IMAGE);
                        grabber->retrieve(rgb_frame);

                        #pragma omp critical(buffer)
                        {
                            depth_buff.push(&depth_frame);
                            color_buff.push(&rgb_frame);
                        }
                    }
                }

                #pragma omp section
                {
                    chrono::system_clock::time_point now = std::chrono::system_clock::now();
                    time_t time = std::chrono::system_clock::to_time_t(now);
                    chrono::milliseconds  ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
                    std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

                    if(!depth_buff.empty()){
                        //Write the image
                        stringstream ss;
                        //ss << "depth_" << ++counter_depth << ".png";
                        ss << "depth_" << put_time(localtime(&time), "%H:%M:%S:") << ms.count() << ".png";
                        imwrite(ss.str(), *(depth_buff.front()));
                        //#pragma omp atomic
                        depth_buff.pop();
                    }

                    if(!color_buff.empty()){
                        //Write again
                        stringstream ss;
                        //ss << "rgb_" << ++counter_color << ".png";
                        ss << "rgb_" << put_time(localtime(&time), "%H:%M:%S:") << ms.count() << ".png";
                        imwrite(ss.str(), *(color_buff.front()));
                        //#pragma omp atomic
                        color_buff.pop();
                    }
                    if(SIGINT_RAISED && depth_buff.empty() && color_buff.empty()) go = false;
                }
            }
        }
    }
}
