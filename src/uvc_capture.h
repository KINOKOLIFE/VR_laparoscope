#pragma once
#include "ofApp.h"

class capture_thread : public ofThread {
public:
    int cameraID;
    ofVideoGrabber cam; // the cam
    vector<ofVideoDevice> devices ;
    std::vector<string> devices_list;
    ofPixels pixels;
    int width, height;
    bool setup;
    ofImage image;
  
    void get_camera_list(){
        devices = cam.listDevices();
        const char* listbox_items[devices.size()];
        for(size_t i = 0; i < devices.size(); i++){
            if(devices[i].bAvailable){
                if(devices[i].deviceName.length() > 10){
                    string str = devices[i].deviceName.substr(0,11);
                    devices_list.push_back( str.c_str() );
                }else{
                    devices_list.push_back( (devices[i].deviceName).c_str() );
                }
                //devices_list.push_back( (devices[i].deviceName).c_str() );//https://www.sejuku.net/blog/52403
            }
        }
        image.load("MATSUMOTO.jpeg");
        make_test_pattern();
    }
    void make_test_pattern(){
        ofFbo f;
        f.allocate(960,540);
        f.begin();
        ofClear(0);
        image.draw(0,0);
        f.end();
        f.readToPixels(pixels);
    }
    void set_param(int index, int _width, int _height){
        cam.close();
        cameraID = index;
        width = _width;
        height = _height;
        cam.setDeviceID(index);
        cam.setup(_width, _height, false);//第３引数を入れないと落ちる。
        setup = false;
        
    }
    void stop_cam(){
        cam.close();
        make_test_pattern();
    }
    void draw(ofFbo &fbo){
        fbo.begin();{
            cam.draw(-10,-100);
        }fbo.end();
    }
    void capture(){
        std::cout<<"capture!"<<std::endl;
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
   
};
