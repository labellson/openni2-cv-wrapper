#include "openni2_cv_grabber.h"

Openni2_grabber::Openni2_grabber(){
    init_openni();
    init_device();
}

Openni2_grabber::~Openni2_grabber(){

}

void Openni2_grabber::init_openni(){
    auto rc = OpenNI::initialize();
    if(rc != STATUS_OK) { 
        cout << "Initialize failed" << endl << OpenNI::getExtendedError() << endl;
        exit(0);
    }
}

void Openni2_grabber::init_device(){
    device_ = new Device();
    auto rc = device_->open(ANY_DEVICE);
    if(rc != STATUS_OK){
        cout << "Couldn't open device" << endl << OpenNI::getExtendedError() << endl;
        exit(0);
    }
}

void Openni2_grabber::init_ir_stream(){
    ir_stream_ = new VideoStream();

    // Create ir stream
    if(device_->getSensorInfo(SENSOR_IR) != nullptr){
        auto rc = ir_stream_->create(*device_, SENSOR_IR);
        if(rc != STATUS_OK){
            cout << "Couldn't create ir stream" << endl << OpenNI::getExtendedError() << endl;
            exit(0);
        }
    }

    // Start the ir stream
    auto rc = ir_stream_->start();
    if(rc != STATUS_OK){
        cout << "Couldn't start the ir stream" << endl << OpenNI::getExtendedError() << endl;
        exit(0);
    }

    ir_frame_ = new VideoFrameRef();

}

void Openni2_grabber::init_depth_stream(){
    depth_stream_ = new VideoStream();

    //Create depth stream
    if(device_->getSensorInfo(SENSOR_DEPTH) != nullptr){
        auto rc = depth_stream_->create(*device_, SENSOR_DEPTH);
        if(rc != STATUS_OK){
            cout << "Couldn't start the depth stream" << endl << OpenNI::getExtendedError() << endl;
            exit(0);
        }
    }

    //Start the depth stream
    auto rc = depth_stream_->start();
    if(rc != STATUS_OK){
        cout << "Couldn't start the depth stream" << endl << OpenNI::getExtendedError() << endl;
        exit(0);
    }
    
    depth_frame_ = new VideoFrameRef();
}

void Openni2_grabber::init_rgb_stream(){
    color_stream_ = new VideoStream();

    //Create color stream
    if(device_->getSensorInfo(SENSOR_COLOR) != nullptr){
        auto rc = color_stream_->create(*device_, SENSOR_COLOR);
        if(rc != STATUS_OK){
            cout << "Couldn't start the color stream" << endl << OpenNI::getExtendedError() << endl;
            exit(0);
        }
    }

    //Start the color stream
    auto rc = color_stream_->start();
    if(rc != STATUS_OK){
        cout << "Couldn't start rhe color stream" << endl << OpenNI::getExtendedError() << endl;
        exit(0);
    }

    color_frame_ = new VideoFrameRef();
}

void Openni2_grabber::retrieve(Mat &f, int sensor_type){
    switch(sensor_type){
        case IR_IMAGE:
            read_ir_frame();
            if(!ir_frame_->isValid()) break;
            parse_uint16_data(f, ir_frame_);
            break;
        case DEPTH_IMAGE:
            read_depth_frame();
            if(!depth_frame_->isValid()) break;
            parse_uint16_data(f, depth_frame_);
            break;
        case RGB_IMAGE:
            read_color_frame();
            if(!color_frame_->isValid()) break;
            parse_uchar_data(f, color_frame_);
            break;
    }
}

void Openni2_grabber::read_ir_frame(){
    read_frame_from_stream(ir_stream_, ir_frame_);
}

void Openni2_grabber::read_depth_frame(){
    read_frame_from_stream(depth_stream_, depth_frame_);
}

void Openni2_grabber::read_color_frame(){
    read_frame_from_stream(color_stream_, color_frame_);
}

void Openni2_grabber::read_frame_from_stream(VideoStream *vstream, VideoFrameRef *vfref){
    auto rc = vstream->readFrame(vfref);
    if(rc != STATUS_OK) cout << "Read failed!" << endl << OpenNI::getExtendedError() << endl;
}

void Openni2_grabber::parse_uint16_data(Mat &f, VideoFrameRef *vfref){
    const uint16_t *buf_ptr = (const uint16_t*) vfref->getData();
    f.create(vfref->getHeight(), vfref->getWidth(), CV_16U);
    memcpy(f.data, buf_ptr, vfref->getHeight() * vfref->getWidth() * sizeof(uint16_t));
}

void Openni2_grabber::parse_uchar_data(Mat &f, VideoFrameRef *vfref){
    const unsigned char *buf_ptr = (const unsigned char*) vfref->getData();
    f.create(vfref->getHeight(), vfref->getWidth(), CV_8UC3);
    memcpy(f.data, buf_ptr, vfref->getHeight() * vfref->getWidth() * 3 * sizeof(unsigned char));
}
