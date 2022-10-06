#pragma once
#include "ofApp.h"
struct imageStack{
    std::vector<glm::vec4> point;
    int px;
    int py;
    int w;
    int h;
    float mm_pixel; // mm/pixel
    bool show;
    bool select;
    std::vector<ofImage> image;
};
