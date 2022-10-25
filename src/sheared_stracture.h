#pragma once
#include "ofApp.h"
struct imageStack{
    std::vector<glm::vec4> point;
    int px;
    int py;
    int w;
    int h;
    
    float gauge0;
    float gauge1;
    float gauge2;
   
    float mm_pixel; // mm/pixel
    bool show;
    bool select;
    string fileName;
    ofImage image;
 
    ofFbo fbo;
    Eigen::Matrix4d mat;
};
