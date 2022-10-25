#pragma once
#include <iostream>
#include <vector>
#include "ofApp.h"
#include "sheared_stracture.h"
class Image_stacker{
public:
    int snapCounter = 0;
    int page_num;
    int current_page ;
    bool live = true;
    vector<imageStack> stkimgs;
    float threshold;
    
    Image_stacker(){
        
    }
    void setDirectory(string s){
        
    }
    void stack(ofImage &img){
        string fileName = "snapshot/snapshot_"+ofToString(snapCounter, 5, '0')+".png";
        if(img.save(fileName)){
            snapCounter ++;
        }else{
            std::cout<<"err"<<std::endl;
        };
    }
    void delete_image(){
        stkimgs.erase(stkimgs.begin() + current_page, stkimgs.begin() + current_page + 1);
        current_page --;
        if( current_page < 0){
            current_page = 0;
        }

    }
    void switch_(){
        live = !live;
    }
    void forward(){
        current_page ++;
        if(current_page > stkimgs.size() - 1){
            current_page = stkimgs.size() - 1;
        }
        std::cout<<stkimgs.size()<<" "<<current_page<<std::endl;
    }
    void backward(){
        current_page --;
        if( current_page < 0){
            current_page = 0;
        }

        std::cout<<stkimgs.size()<<" "<<current_page<<std::endl;
    }
    ofImage get_image(){
        //cout << stkimgs[current_page].fileName << endl;
        ofImage i;
        i.load(stkimgs[current_page].fileName);
        return i;
    }
    imageStack get_stack(){
        return stkimgs[current_page];
    }
    void stack(ofImage &img, imageStack &stk, Eigen::Matrix4d mat){
        string fileName = "snapshot/snapshot_"+ofToString(snapCounter, 5, '0')+".png";
        if(img.save(fileName)){
            
            current_page = stkimgs.size() - 1;
            snapCounter ++;
            //-----
            ofFbo f;
            ofDisableArbTex();
            stk.fbo.allocate(240,85);
            stk.fbo.begin();
            img.draw(0,0,240,85);
            stk.fbo.end();
            stk.show = true;
            stk.select = true;
            stk.mat << mat;
            stk.fileName = fileName;
            //----
            stkimgs.push_back(stk);
        }else{
            std::cout<<"err"<<std::endl;
        };
        
    }
    void get_feed(ofImage &feed, ofPixels &uvc, vector<imageStack> &stks, imageStack &stk){
        
        
    }
    void crop(){
        if(stkimgs.size() > 0){
            float mm_pixel = abs(stkimgs[current_page].gauge0 -  stkimgs[current_page].gauge1 )/ 10.0;  // 10mm =  mm_pixel
            float cropwidth = stkimgs[current_page].w / 480.0 *  1280.0;
            float crophight = stkimgs[current_page].h / 270.0 *  720.0;
            float croppx = stkimgs[current_page].px / 480.0 *  1280.0;;
            float croppy = stkimgs[current_page].py / 270.0 *  720.0;
            float meshWidth = cropwidth * mm_pixel;
            float meshHeight = crophight * mm_pixel;
            ofImage i;
            i.load(stkimgs[current_page].fileName);
           
            ofFbo f;
            ofDisableArbTex();
            f.allocate(cropwidth,crophight);
            f.begin();
            ofClear(0,0);
            i.draw(-croppx, -croppy);
            f.end();
            ofDisableArbTex();
            stkimgs[current_page].fbo.allocate(meshWidth, meshHeight);
            stkimgs[current_page].fbo.begin();
            ofClear(0,0);
            f.draw(0,0, meshWidth, meshHeight);
            stkimgs[current_page].fbo.end();
        /*
            ofDisableArbTex();
            stkimgs[current_page].fbo.allocate(cropwidth,crophight);
            stkimgs[current_page].fbo.begin();
            i.draw(-croppx, -croppy);
            stkimgs[current_page].fbo.end();
             */
        }
    }
    void translucent(){
        if(stkimgs.size() > 0){
            ofPixels pixels;
            stkimgs[current_page].fbo.readToPixels(pixels);
            for(int x = 0; x < pixels.getWidth(); x++ ){
                for(int y = 0; y < pixels.getHeight(); y++ ){
                    ofColor c = pixels.getColor(x,y);
                    if( c.r + c.g + c.b < threshold){
                        ofColor c = ofColor(0,0,0,0);
                        pixels.setColor(x, y, c);
                    }
                }
            }
            ofImage i;
            i.setFromPixels(pixels);
            stkimgs[current_page].fbo.begin();
            ofClear(0,0);
            i.draw(0,0);
            stkimgs[current_page].fbo.end();
        }
    }
};
