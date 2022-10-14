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
#include "ultrasound_viewer.h"
#include "sheared_stracture.h"
#include "uvc_capture.h"
#include "graphics.h"
#include "realsense.h"
#include "hid.h"
#include "aruco_manager.h"
#include "objLoader.h"

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
    //realsense
    rs265 *rs;
    
    //gbuffer
    ofMesh quad; // 表示用板ポリ
    ofFbo g_buffer;//https://qiita.com/y_UM4/items/7647fd9fc19e60ec5822 Deferred Rendering
    ofMatrix4x4 projection_matrix;
    ofShader geo_shader;
    ofShader phong_shader;
    ofFbo gfbo;
    void gbuffer_setup();
    //---hidapi
    void button_1_drag(bool &b);
    void button_1_up(bool &b);
    void button_2_up(bool &b);
    void button_3_up(bool &b);
    hid *HID;
    bool hid_setup;
    //------endoscope
    float fov = 60.0f;
    //testthread *tst;
    //------aruco
    aruco_manager aruco_manager_();
    vector<objLoader> objs;
};
