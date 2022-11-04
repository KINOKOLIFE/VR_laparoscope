#pragma once
#include "ofApp.h"

struct MeshConatiner{
    ofMesh mesh;
    glm::mat4 matrix;
    ofFbo fbo;
    glm::vec4 color;
    bool use_tecxture;
};
struct meshContainer{
    ofMesh mesh;
    glm::mat4 matrix;
    glm::vec4 color;
    string given_name;
    string parent_name;
    bool dysplay;
    bool parent_isDysplay;
    bool pick_target;
    bool useasskin;
    int camera_port_vertex;
    int forceps1_port_vertex;
    int forceps2_port_vertex;
};
