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
    grabber->init_rgb_stream();

    Mat ir_frame, depth_frame, rgb_frame;
    int counter = 0;
    bool go = true;
    
    //Lets Rock and grab data
    while(go){
        grabber->retrieve(ir_frame, Openni2_grabber::IR_IMAGE);
        grabber->retrieve(depth_frame, Openni2_grabber::DEPTH_IMAGE);
        grabber->retrieve(rgb_frame, Openni2_grabber::RGB_IMAGE);
        imshow("ir_raw", ir_frame);
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
    }
}
