#pragma once
#include "ofApp.h"


class Controller{
public:
    static bool enable_picker;
    static int nearlest_vertex_from_picker;
    static glm::mat4 offset_1;
    static glm::mat4 offset_1_onDrag;
    static glm::mat4 offset_2;
    static glm::mat4 offset_2_onDrag;
};
