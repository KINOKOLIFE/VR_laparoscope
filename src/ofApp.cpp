#include "ofApp.h"
Viewer viewer(0 ,0 ,480, 270);
ofImage fisheye_left_image;
ofxAssimpModelLoader realsense_model;
std::vector<ofFbo> fboes;
ofMesh realsense_mesh;
ofMesh drawing_plane;


//--------------------------------------------------------------
void ofApp::setup(){
    viewer.fbo_allocate(fbo);
    //------imGUI セットアップ
    gui.setup();
    //--video capture
    uvc_cap.get_camera_list();
    //3d perspective
    perspective.allocate(424, 400);
    easycam.setControlArea(area2);
    //--
    fisheye_left_image.allocate(848, 800, OF_IMAGE_GRAYSCALE);
    //--
    realsense_model.loadModel("realsense_model/t265.obj",true);
    realsense_model.setScaleNormalization(false);

    realsense_mesh = realsense_model.getMesh(0);;
    setNormals(realsense_mesh);
    //ofEnableSmoothing();
    ofDisableArbTex();
    //---human interface device
    HID = new hid();
    ofAddListener(HID->dragging_button1, this, &ofApp::button_1_drag);
    ofAddListener(HID->up_button1, this, &ofApp::button_1_up);
    ofAddListener(HID->up_button2, this, &ofApp::button_2_up);
    ofAddListener(HID->up_button3, this, &ofApp::button_3_up);
    hid_setup = HID->setup();
    if(hid_setup){
        HID->startThread();
    }
    //--gbuffer
    gbuffer_setup();
    drawing_plane = rectMesh(0,0,100,100,true);
    setNormals(drawing_plane);
    //--endoscope
    //float camHeight = 540;
    //float camera_matirx_height = 427;
    //fov = 2.0 * atan( camHeight / 2.0 / camera_matirx_height ) / M_PI * 180;
    // screen height /2.0   :   fy    -> most important code
    //cam.setFov(fov);
   //--gizmo
    gizmocam.setControlArea(area2);


}
//--------------------------------------------------------------
void ofApp::update(){

    mygizmo.unable_easycam(easycam);
    
    viewer.update(fbo);
    viewer.hover = false;
    drawTerrios(perspective, easycam);
 
    ofMesh m;
    m = rectMesh(0,0,100,100,true);
    setNormals(m);
    mygizmo.update(gizmocam);
    perspective.begin();{
        easycam.begin();{
            ofPushMatrix();{
                if(uvc_cap.setup){
                    UVCimage.getTexture().bind();
                    m.draw();
                    UVCimage.getTexture().unbind();
                }
                for(int i = 0; i < objs.size(); i++){
                    ofLight l;
                    l.enable();
                    objs[i].model.setPosition(0, 0, 250);
                    objs[i].model.drawFaces();
                }
                ofMultMatrix(mygizmo.model);
                ofDrawBox(40);
            }ofPopMatrix();
        }easycam.end();
    }perspective.end();
    
  
    if(rs){
        draw_model(perspective, easycam, realsense_model, rs->t265_rawoutput, ofColor(200));
        //fisheye_left_image.setFromPixels(rs->fisheye_left_cvimage.ptr(), 848, 800, OF_IMAGE_GRAYSCALE);
        fisheye_left_image.setFromPixels(rs->colorimg.ptr(), 848, 800, OF_IMAGE_COLOR);
        if(rs->tvecs_.size()>0){
            //std::cout<<tst->tvecs_[0]<<std::endl;
        }
    }
    gfbo.begin();{
        ofClear(0);
        glEnable(GL_DEPTH_TEST);
        //1. render geometry to G-Buffer-------------------------------
        ofMatrix4x4 viewMatrix;
        g_buffer.begin();{
            g_buffer.activateAllDrawBuffers();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            easycam.begin();{
                projection_matrix = easycam.getProjectionMatrix();
                viewMatrix = ofGetCurrentViewMatrix();
                geo_shader.begin();{
                    geo_shader.setUniformMatrix4f("view", viewMatrix);
                    geo_shader.setUniformMatrix4f("projection", projection_matrix);
                    geo_shader.setUniform1i("u_isLight", 0);
          
                    int deg = 0;
                    for(auto &f:fboes){
                        ofPushMatrix();{
                            ofRotateXDeg(deg);
                            deg += 30;
                            ofTranslate(0,-0.5,0);
                            geo_shader.setUniformTexture("image", f.getTexture(), 0);
                            drawing_plane.draw();
                        }ofPopMatrix();
                    }
   
                    geo_shader.setUniform1i("u_isLight", 1);
                    geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5 ,1.0));
                    realsense_mesh.draw();
                    ofMesh m;
                    geo_shader.setUniform4f("color" , glm::vec4(1.0 ,0.0 ,0.0 ,1.0));
                    m = rectMesh(0,0,100,100,true);
                    setNormals(m);
                    m.draw();
                    ofPushMatrix();
                    ofTranslate(0,0, 250);
                    geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5,1.0));
                 
                    for(int i = 0; i < objs.size(); i++){
                        for(int j = 0; j < objs[i].mesh_.size(); j++){
                            //geo_shader.setUniform4f("color" , glm::vec4(0.5 + j * 0.1 ,1.0 - j * 0.1 ,1.0 ,1.0));
                            objs[i].mesh_[j].drawFaces();
                        }
                    }
                 
                    ofPopMatrix();
                    geo_shader.setUniform4f("color" , glm::vec4(0.4 ,0.4 ,0.4 ,0.6));
                    //ofDrawGrid(10,10,false,true,false,false);
                    ofDrawGrid(10,10,false,false,false,true);
                    ofPushMatrix();
                    ofTranslate(0.1,0.1,0.1);
                    geo_shader.setUniform4f("color" , glm::vec4(1.0 ,0.0 ,0.0 ,1.0));
                    ofDrawLine(0,0,0,100,0,0);
                    geo_shader.setUniform4f("color" , glm::vec4(0.0 ,1.0 ,0.0 ,1.0));
                    ofDrawLine(0,0,0,0,100,0);
                    geo_shader.setUniform4f("color" , glm::vec4(0.0 ,0.0 ,1.0 ,1.0));
                    ofDrawLine(0,0,0,0,0,100);
                    ofPopMatrix();
                }geo_shader.end();
            }easycam.end();
        }g_buffer.end();
        // 3. lighting Pass--------------------------------------------
        glDisable(GL_DEPTH_TEST);
        phong_shader.begin();{
            phong_shader.setUniformTexture("gPosition", g_buffer.getTexture(0), 0);
            phong_shader.setUniformTexture("gNormal", g_buffer.getTexture(1), 1);
            phong_shader.setUniformTexture("gAlbedo", g_buffer.getTexture(2), 2);
            phong_shader.setUniform3f("u_lightPos", ofVec3f(20.0, 50.0, 30.0) * viewMatrix);
            phong_shader.setUniform3f("viewPos", easycam.getPosition());
            quad.draw();
        }phong_shader.end();
    }gfbo.end();
    
    
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
    gfbo.draw(480,400);
    gui_draw();
}
//--------------------------------------------------------------
void ofApp::button_1_drag(bool &b){
    std::cout<<"drag button1 call back"<<std::endl;
}
//--------------------------------------------------------------
void ofApp::button_1_up(bool &b){
    std::cout<<"up button1 call back"<<std::endl;
}
//--------------------------------------------------------------
void ofApp::button_2_up(bool &b){
    std::cout<<"up button2 call back"<<std::endl;
}
//--------------------------------------------------------------
void ofApp::button_3_up(bool &b){
    std::cout<<"up button3 call back"<<std::endl;
}
//--------------------------------------------------------------
void ofApp::exit(){
    viewer.exit();
    uvc_cap.stopThread();
    if(rs){
        rs->stopThread();
        delete rs;
    }
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
            const char* listbox_items[uvc_cap.devices_list.size()];
            for(size_t i = 0; i < uvc_cap.devices_list.size(); i++){
                    listbox_items[i] = uvc_cap.devices_list[i].c_str();//https://www.sejuku.net/blog/52403
            }
            //https://qiita.com/ousttrue/items/ae7c8d5715adffc5b1fa
            static int listbox_item_current = 0;
            if(ImGui::ListBox("", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 3)){
                string s = listbox_items[listbox_item_current];
                if( std::equal(s.begin() , s.begin() + 7, "H264 USB") ){
                    uvc_cap.set_param(listbox_item_current,1280,720);
                }
                if( std::equal(s.begin() , s.begin() + 9, "FaceTime HD") ){
                    uvc_cap.set_param(listbox_item_current,1280,720);
                }
                if(!uvc_cap.isThreadRunning()){
                    uvc_cap.startThread();
                }
            }
            if(uvc_cap.setup){
                if (ImGui::Button(" capture ")) {
                    uvc_cap.capture();
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
        ImGui::Begin("read CT model");{
            if(ImGui::IsWindowHovered()){
                viewer.hover = true;
                easycam.disableMouseInput();
            }
            if (ImGui::Button(" open obj file ")) {
                ofFileDialogResult result = ofSystemLoadDialog("Load file");
                if(result.bSuccess) {
         
                    string path = result.getPath();
                    ofxAssimpModelLoader model_segmentaion;
                    objLoader o = objLoader(path);
                    if(o.set){
                        objs.push_back(o);
                    }
                }
            }
            //static float col2[3] = { 1.0f,0.0f,0.2f };
            
            for(int i; i < objs.size(); i++){
                if(objs[i].show){
                    if (ImGui::Button(" hide ")) {
                        objs[i].show = false;
                    }
                }else{
                    if (ImGui::Button(" show ")) {
                        objs[i].show = true;
                    }
                }
                char* c = const_cast<char*>(objs[i].thum.c_str());
                ImGui::SameLine();ImGui::Text(c);
                
                //static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
                //ImGui::ColorEdit3("color", col2);
                //ImGui::ColorEdit3("color", col3);
            }
            
        }ImGui::End();
        ImGui::Begin("endoscope");{
            if(ImGui::IsWindowHovered()){
                viewer.hover = true; // GUI上にマウスがあるときにcropウインドウの操作をキャンセルさせるため
                easycam.disableMouseInput();
            }
            if (ImGui::Button("on")) {
                //easycamEnable = !easycamEnable;
            }
            if(ImGui::SliderFloat("Float", &fov, 40.0f, 120.0f)){
                easycam.setFov(fov);
            }
        }ImGui::End();
    }gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    mygizmo.enable_gozmo(gizmocam);
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
    //-------------透過画像を準備
    ofDisableArbTex();
    for(int i = 0; i < 10; i++){
        ofFbo f;
        f.allocate(256,256);
        f.begin();
        ofClear(0,0);
        for(int k = 0; k < 10; k++)
        {
            ofSetColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255),ofRandom(200,255));
            ofDrawCircle(ofRandom(0,255), ofRandom(0,255), 10);
        }
        f.end();
        fboes.push_back(f);
    }
    //-----------------------
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
//-----------------------

