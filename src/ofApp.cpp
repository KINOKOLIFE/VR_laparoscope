#include "ofApp.h"
Viewer viewer(0 ,0 ,480, 270);
ofImage fisheye_left_image;
ofxAssimpModelLoader realsense_model;
//--------------------------------------------------------------
void ofApp::setup(){
    viewer.fbo_allocate(fbo);
    //------imGUI セットアップ
    gui.setup();
    //--video capture
    uvc_cap.get_camera_list(uvc_list);
    //3d perspective
    perspective.allocate(848, 400);
    easycam.setControlArea(area2);
    //--
    fisheye_left_image.allocate(848, 800, OF_IMAGE_GRAYSCALE);
    //--
    realsense_model.loadModel("realsense_model/t265.obj",true);
    realsense_model.setScaleNormalization(false);
    //ofEnableSmoothing();
}

//--------------------------------------------------------------
void ofApp::update(){
    viewer.update(fbo);
    viewer.hover = false;
    easycam.enableMouseInput();
    drawTerrios(perspective, easycam);
    if(rs){
        draw_model(perspective, easycam, realsense_model, rs->t265_rawoutput, ofColor(200));
        fisheye_left_image.setFromPixels(rs->fisheye_left_cvimage.ptr(), 848, 800, OF_IMAGE_GRAYSCALE);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDisableDepthTest();
    glDisable(GL_LIGHTING);
    fisheye_left_image.draw(480,0,424,400);
    if(uvc_cap.setup){
        UVCimage.setFromPixels(uvc_cap.pixels);
        UVCimage.draw(viewer.px,  viewer.py, viewer.width, viewer.height);
    }
    fbo.draw(viewer.px,  viewer.py, viewer.width, viewer.height);
    perspective.draw(area2);
    gui_draw();
}
//--------------------------------------------------------------
void ofApp::exit(){
    uvc_cap.stopThread();
    if(rs){
        rs->stopThread();
    }
    delete rs;
}
//--------------------------------------------------------------
void ofApp::gui_draw(){
    gui.begin();{
        ImGui::Begin("ultrasound");{
            if(ImGui::IsWindowHovered()){
                viewer.hover = true;
                easycam.disableMouseInput();
            }
            //ImGui::Checkbox("flip", &isEnable);//https://qiita.com/Ushio/items/446d78c881334919e156
            static char buf[256] = "";//http://www.sanko-shoko.net/note.php?id=qlv3
            if (ImGui::InputText("markup", buf, 256)) {
               
            }
            static float col1[3] = { 1.0f,0.0f,0.2f };
            //static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            ImGui::ColorEdit3("color 1", col1);
            //ImGui::SameLine();
            if (ImGui::Button("delete")) {

            }
            ImGui::SameLine();if (ImGui::Button("delete all points")) {

            }
            ImGui::SameLine();if (ImGui::Button("delete plane")) {

            }

        }ImGui::End();
        ImGui::Begin("UVC source");{
            if(ImGui::IsWindowHovered()){
                viewer.hover = true; // GUI上にマウスがあるときにcropウインドウの操作をキャンセルさせるため
                easycam.disableMouseInput();
            }else{
                
            }
            const char* listbox_items[uvc_list.size()];
            for(size_t i = 0; i < uvc_list.size(); i++){
                    listbox_items[i] = uvc_list[i].c_str();//https://www.sejuku.net/blog/52403
            }
            //https://qiita.com/ousttrue/items/ae7c8d5715adffc5b1fa
            static int listbox_item_current = 0;
            if(ImGui::ListBox("", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 3)){
                string s = listbox_items[listbox_item_current];
                if( s == "H264 USB Camera"){
                    uvc_cap.set_param(listbox_item_current,1280,720);
                }
                
                if(!uvc_cap.isThreadRunning()){
                    uvc_cap.startThread();
                }
            }
            ImGui::Text("Hello");
        }ImGui::End();
        ImGui::Begin("real sense t265");{
            if(ImGui::IsWindowHovered()){
                viewer.hover = true; // GUI上にマウスがあるときにcropウインドウの操作をキャンセルさせるため
                easycam.disableMouseInput();
            }
            if(rs == nullptr){
                if (ImGui::Button(" connect ")) {
                    rs = new rs265();
                    rs->setup();
                    if(rs->connect()){
                        rs->startThread();
                    }else{
                        delete rs;
                        rs = nullptr;
                    }
                }
            }
            if(rs){
                ImGui::SliderFloat("process", &rs->iProcessCov, 0.00f, 200.0f);
                ImGui::SliderFloat("measure", &rs->iMeasurementCov, 0.00f, 200.0f);
                if (ImGui::Button(" kill ")) {
                    rs->stopThread();
                    delete rs;
                    rs = nullptr;
                }
            }
        }ImGui::End();
    }gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    viewer.mouse(x, y, 0, 0);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    viewer.mouse(x, y, 0, 1);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    viewer.mouse(x, y, 0, 2);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    viewer.mouse(x, y, 0, 3);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
//--------------------------------------------------------------
void ofApp::gbuffer_setup(){
    //-------shader
    geo_shader.load("shader/geo_shader");
    phong_shader.load("shader/phong_shader");
    gfbo.allocate(480, 270);
    //-------------
    ofDisableArbTex();
    vector<GLint> formats = { GL_RGBA16F, GL_RGBA16F, GL_RGBA16F };
    ofFbo::Settings settings;
    settings.width = 480;
    settings.height = 270;
    settings.textureTarget = GL_TEXTURE_2D;
    settings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    settings.wrapModeVertical = GL_CLAMP_TO_EDGE;
    settings.minFilter = GL_NEAREST;
    settings.maxFilter = GL_NEAREST;
    settings.numColorbuffers = 3;
    settings.colorFormats = formats;
    settings.numSamples = 4;
    settings.useDepth = true;
    settings.useStencil = true;
    g_buffer.allocate(settings);
    g_buffer.checkStatus();
    ofEnableArbTex();
    
    quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    quad.addVertex(ofVec3f(1.0, 1.0, 0.0)); // top-right
    quad.addTexCoord(ofVec2f(1.0f, 0.0f));
    quad.addVertex(ofVec3f(1.0, -1.0, 0.0)); //bottom-right
    quad.addTexCoord(ofVec2f(1.0f, 1.0f));
    quad.addVertex(ofVec3f(-1.0, -1.0, 0.0)); //bottom-left
    quad.addTexCoord(ofVec2f(0.0f, 1.0f));
    quad.addVertex(ofVec3f(-1.0, 1.0, 0.0)); //top-left
    quad.addTexCoord(ofVec2f(0.0f, 0.0f));
}
