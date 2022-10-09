#pragma once
#include "ofApp.h"
#include <hidapi/hidapi.h>
class hid : public ofThread {
public:
    ofEvent<bool> dragging_button1;
    ofEvent<bool> up_button1;
    ofEvent<bool> up_button2;
    ofEvent<bool> up_button3;
    bool button_1_state = false;
    bool button_2_state = false;
    bool button_3_state = false;
    hid_device *handle;
    unsigned char buf[4];
    bool setup(){
        int res = hid_init();
        handle = hid_open(0xE502, 0xBBAB, NULL);
        wchar_t s[] = L"98-CD-AC-D3-56-76";
        if(handle == NULL){
            ofLog(OF_LOG_NOTICE) << "fail to connect BLE ";
            return false;
        }else{
            ofLog(OF_LOG_NOTICE) << "connect BLE ";
            wchar_t wstr[64];
            int res = hid_get_serial_number_string(handle, wstr, 64);
            wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);
            return true;
        }
    }
    void threadedFunction() {
        while(isThreadRunning()) {
            int res = hid_read(handle, buf, 4);
            //button1
            if ((int)buf[1] & (1<<0)){
                if(!button_1_state){
                    ofNotifyEvent(dragging_button1, button_1_state);
                }
                button_1_state = true;
                
            }else{
                if(button_1_state){
                    ofNotifyEvent(up_button1, button_1_state);
                }
                button_1_state = false;
            }
            //buntton2
            if ((int)buf[1] & (1<<1)){
                if(!button_2_state){
                }
                button_2_state = true;
                
            }else{
                if(button_2_state){
                    ofNotifyEvent(up_button2, button_2_state);
                }
                button_2_state = false;
            }
            //button3
            if ((int)buf[1] & (1<<2)){
                if(!button_3_state){
                }
                button_3_state = true;
                
            }else{
                if(button_3_state){
                    ofNotifyEvent(up_button3, button_3_state);
                }
                button_3_state = false;
            }
            //printf("buf[%d]: %d\n", 1, buf[1]);
        }
    }
    void stopThread(){
        hid_close(handle);
        std::cout<<"close1"<<std::endl;
        int res = hid_exit();
        std::cout<<"close2"<<std::endl;
    }
};
