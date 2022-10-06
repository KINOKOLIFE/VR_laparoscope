#pragma once
#include "ofApp.h"

class capture_thread : public ofThread {
public:
    void get_camera_list(std::vector<string> &list){
        devices = cam.listDevices();
        const char* listbox_items[devices.size()];
        for(size_t i = 0; i < devices.size(); i++){
            if(devices[i].bAvailable){
                list.push_back( (devices[i].deviceName).c_str() );//https://www.sejuku.net/blog/52403
            }
        }
    }
    void set_param(int index, int _width, int _height){
        cam.close();
        cameraID = index;
        width = _width;
        height = _height;
        cam.setDeviceID(index);
        cam.initGrabber(_width, _height, false);//第３引数を入れないと落ちる。
        setup = false;
    }
    void draw(ofFbo &fbo){
        fbo.begin();{
            cam.draw(-10,-100);
        }fbo.end();
    }
    void threadedFunction() {
        while(isThreadRunning()) {
            cam.update();
            if(cam.isFrameNew()) {
                setup = true;
                lock();{
                    pixels = cam.getPixels();
                }unlock();
            }
        }
    }
    int cameraID;
    ofVideoGrabber cam; // the cam
    vector<ofVideoDevice> devices ;
    ofPixels pixels;
    int width, height;
    bool setup;
};
