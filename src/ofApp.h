#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ultrasound_viewer.h"
#include "sheared_stracture.h"
#include "uvc_capture.h"

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
    //----uvc camera
    std::vector<string> uvc_list;
    ofImage UVCimage;
    capture_thread uvc_cap;
};
