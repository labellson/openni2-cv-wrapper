#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <queue>
#include <omp.h>
#include "openni2_cv_grabber.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv){
    Openni2_grabber *grabber = new Openni2_grabber();
    grabber->init_depth_stream();
    grabber->init_rgb_stream();

    Mat depth_frame, rgb_frame;
    queue<Mat*> color_buff, depth_buff;
    int counter_depth = -1, counter_color = -1;
    bool go = true;
    
    //Lets Rock and grab data
    /*while(go){

        grabber->retrieve(depth_frame, Openni2_grabber::DEPTH_IMAGE);
        grabber->retrieve(rgb_frame, Openni2_grabber::RGB_IMAGE);
        imshow("depth_raw", depth_frame);
        imshow("rgb_raw", rgb_frame);
        switch(waitKey(30)){
            case 27:
                go = false;
                break;
            case 32:
                cout << "Saved frame " << ++counter << endl;
                stringstream ss;
                ss << "rgb_" << counter << ".png";
                imwrite(ss.str(), rgb_frame);
                stringstream ss1;
                ss1 << "depth_" << counter << ".png";
                imwrite(ss1.str(), depth_frame);
                break;
        }
    }*/
    #pragma omp parallel shared(go, color_buff, depth_buff), num_threads(2)
    {
        while(go){
            #pragma omp sections nowait
            {
                #pragma omp section
                {
                   grabber->retrieve(depth_frame, Openni2_grabber::DEPTH_IMAGE);
                   grabber->retrieve(rgb_frame);

                   #pragma omp critical buffer
                   {
                        depth_buff.push(&depth_frame);
                        color_buff.push(&rgb_frame);
                   }
                }

                #pragma omp section
                {

                    if(!depth_buff.empty()){
                        //Write the image
                        stringstream ss;
                        ss << "depth_" << ++counter_depth << ".png";
                        imwrite(ss.str(), *(depth_buff.front()));
                        depth_buff.pop();
                    }

                    if(!color_buff.empty()){
                        //Write again
                        stringstream ss;
                        ss << "rgb_" << ++counter_color << ".png";
                        imwrite(ss.str(), *(color_buff.front()));
                        color_buff.pop();
                    }
                }
            }
        }
    }
}
