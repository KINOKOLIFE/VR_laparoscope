#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>

#include "ofxImGui.h"
#include "sheared.h"
#include "uvc_capture.h"
#include "graphics.h"
#include "realsense.h"
#include "hid.h"
#include "aruco_manager.h"

#include "mesh_holder.h"
#include "picker.h"
#include "objLoader.h"
#include "gizmo3d.h"
#include "stack_manager.h"
#include "g_buffer.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    //ultrasound_gause
    ofFbo fbo;
    ofxImGui::Gui gui;
    void gui_draw();
    //----uvc camera

    ofImage UVCimage;
    capture_thread uvc_cap;
    //---3d pespective
    ofFbo perspective;
    ofEasyCam easycam;
    ofRectangle area2 = ofRectangle(0,400,424,400);
    //------realsense
    rs265 real_sense;
    //-------hidapi
    void button_1_drag(bool &b);
    void button_1_up(bool &b);
    void button_2_up(bool &b);
    void button_3_up(bool &b);
    hid *HID;
    bool hid_setup;
    //------endoscope
    float fov = 60.0f;
    ofCamera endoscope_camera;
    //------aruco
    aruco_manager aruco_manager_();
    //------mesh
    vector<objLoader> objs;

    meshHolder mesh_holder;
    //-------gizmo_camera
    ofEasyCam gizmocam;
    gizmo3d mygizmo;
    //------g-bffer
    gbuffer geeBuffer;
    //----picker
    Picker picker;
    //---
    aruco_manager arucoManager;
    
};
