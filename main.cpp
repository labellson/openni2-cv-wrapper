#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "openni2_cv_grabber.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv){
    Openni2_grabber *grabber = new Openni2_grabber();
    grabber->init_ir_stream();
    grabber->init_depth_stream();

    Mat ir_frame, depth_frame;
    //Grab Data
    while(waitKey(30) <= 0){
        grabber->retrieve(ir_frame, Openni2_grabber::IR_IMAGE);
        grabber->retrieve(depth_frame, Openni2_grabber::DEPTH_IMAGE);
        imshow("ir_raw", ir_frame);
        imshow("depth_raw", depth_frame);
    }
}
