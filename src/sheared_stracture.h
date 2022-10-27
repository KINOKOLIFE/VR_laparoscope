#pragma once
#include "ofApp.h"

struct MeshConatiner{
    ofMesh mesh;
    glm::mat4 matrix;
    ofFbo fbo;
    glm::vec4 color;
    bool use_tecxture;
};
