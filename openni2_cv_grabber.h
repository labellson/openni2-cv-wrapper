#include <OpenNI.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;
using namespace openni;

//TODO: init_rgb_stream
//TODO: Change device options on init_device, Not Any_device
class Openni2_grabber {

    public:

        Openni2_grabber();

        ~Openni2_grabber();

        void init_ir_stream();

        void init_depth_stream();

        void init_rgb_stream();

        void retrieve(Mat &f, int sensor_type = 0);

        enum {RGB_IMAGE, DEPTH_IMAGE, IR_IMAGE};

    private:

        void init_openni();

        void init_device();

        void read_ir_frame();

        void read_depth_frame();

        void read_color_frame();

        void read_frame_from_stream(VideoStream *vstream, VideoFrameRef *vfref);

        void parse_uint16_data(Mat &f, VideoFrameRef  *vfref);

        void parse_uchar_data(Mat &f, VideoFrameRef *vfref);

        Device *device_;
        VideoStream *depth_stream_;
        VideoStream *color_stream_;
        VideoStream *ir_stream_;
        VideoFrameRef *depth_frame_;
        VideoFrameRef *color_frame_;
        VideoFrameRef *ir_frame_;

};
