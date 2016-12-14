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

struct Frame {
    public:
        Mat f;
        String f_name;
};

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

    queue<Frame> color_buff, depth_buff;
    int counter_depth = -1, counter_color = -1;
    bool go = true;
    
    #pragma omp parallel shared(go, color_buff, depth_buff), num_threads(3)
    {
        while(go){
            #pragma omp sections nowait
            {
                #pragma omp section
                {
                    if(!SIGINT_RAISED){
                        Mat depth_frame, rgb_frame;
                        grabber->retrieve(depth_frame, Openni2_grabber::DEPTH_IMAGE);
                        grabber->retrieve(rgb_frame);

                        chrono::system_clock::time_point now = std::chrono::system_clock::now();
                        time_t time = std::chrono::system_clock::to_time_t(now);
                        chrono::milliseconds  ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
                        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

                        stringstream t_sst, rgb_sst, depth_sst;
                        t_sst << put_time(localtime(&time), "%H_%M_%S_") << setfill('0') << setw(3) << ms.count();
                        rgb_sst << "rgb" << t_sst.str() << ".png";
                        depth_sst << "depth" << t_sst.str() << ".pgm";
                        struct Frame srgb, sdepth;
                        srgb.f = rgb_frame;
                        sdepth.f = depth_frame;
                        srgb.f_name = rgb_sst.str();
                        sdepth.f_name = depth_sst.str();


                        #pragma omp critical(buffer)
                        {
                            depth_buff.push(sdepth);
                            color_buff.push(srgb);
                        }
                    }
                }

                #pragma omp section
                {

                    try {
                        if(!depth_buff.empty()){
                            //Write the image
                            imwrite(depth_buff.front().f_name, depth_buff.front().f);
                            #pragma omp critical(buffer)
                            {
                                depth_buff.pop();
                            }
                        }

                        if(!color_buff.empty()){
                            //Write again
                            imwrite(color_buff.front().f_name, color_buff.front().f);
                            #pragma omp critical(buffer)
                            {
                                color_buff.pop();
                            }
                        }
                    } catch(int e){
                        cout << "Danger: Unexpected exception!!" << endl;
                    }
                    if(SIGINT_RAISED && depth_buff.empty() && color_buff.empty()) go = false;
                }
            }
        }
    }
}
