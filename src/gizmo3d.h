#pragma once
#include "ofApp.h"

class gizmo3d{
public:
    bool enable = false;
    glm::mat4x4 fake_camera_previous;
    glm::mat4x4 delta;
    glm::mat4x4 model_previous;
    glm::mat4x4 model;

    gizmo3d(){
        fake_camera_previous = glm::mat4x4(1.0f);
        delta = glm::mat4x4(1.0f);
        model_previous = glm::mat4x4(1.0f);
        model = glm::mat4x4(1.0f);
    }
    void setup(ofEasyCam &easycam, ofRectangle area){
        easycam.setControlArea(area);
    }
    void update(ofEasyCam &gizmo_camera){
        gizmo_camera.begin();
        if(this->enable){
            delta = glm::inverse(fake_camera_previous) * gizmo_camera.getModelViewMatrix();
            model = model_previous * delta;
        }
        gizmo_camera.end();
    }
    void getModel(glm::mat4x4 &model_){
        model_ = glm::mat4x4(model);
    }
    void enable_gozmo(ofEasyCam &fake_camera){
        if(this->enable){
            this->enable = false;
            model_previous = glm::mat4x4(model);
            delta = glm::mat4x4(1.0f);
            
        }else{
            this->enable = true;
            fake_camera_previous = glm::mat4x4(fake_camera.getModelViewMatrix());
        }
    }
    void unable_easycam(ofEasyCam &perspective_camera){
        if(this->enable){
            perspective_camera.disableMouseInput();
        }else{
            perspective_camera.enableMouseInput();
        }
    }
};
/*
 ----GIZMO 3D --------------
 ofRectangle area(400,0,400,400); 適応したいfboのスクリーンへの描画領域
 ofFbo perspective;  適応したいfbo
 ofEasyCam perspective_camera;　適応したいfboのeasycam
 ofEasyCam rotation_camera;　モデリング座標を回転させるためのfake camera
 
//--------------
void ofApp::setup(){
    perspective.allocate(area.getWidth(), area.getHeight());
    perspective_camera.setControlArea(area);
    rotation_camera.setControlArea(area);
}

//--------------------------------------------------------------
void ofApp::update(){
    myRotator.unable_easycam(perspective_camera);//元々（この場合はperspectiveの）easycamをストップさせる。
 
    //--------------
    myRotator.update(rotation_camera);
    //-------------
    ofFill();
    perspective.begin();
        ofClear(0);
        ofBackground(50);
        perspective_camera.begin();
            ofDrawGrid(10,10,true,false,false,true);
            ofDrawAxis(10);
            ofPushMatrix();
                ofMultMatrix(myRotator.model_previous);・・・・初期位置からの変位。
                ofMultMatrix(myRotator.delta);・・・・今回の変位。
（ もう少し洗練できるかも ofMultMatrix(myRotator.dist);とか）
                ofTranslate(50, 50, 50);
                ofDrawBox(100);
            ofPopMatrix();
        perspective_camera.end();
    perspective.end();
   
}

//--------------------------------------------------------------
void ofApp::draw(){
    perspective.draw(area.getX(), area.getY());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    myRotator.enable_gizmo(rotation_camera);//オブジェクトを回転させるモードへ入る時に使用
}

*/
