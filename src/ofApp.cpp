#include "ofApp.h"

ofImage fisheye_left_image;
ofxAssimpModelLoader realsense_model;
//std::vector<ofFbo> fboes;
ofMesh realsense_mesh;
satackviewer::manager viewer;
vector<MeshConatiner> mesh_container_stack;
//----
float near = 10.0;
float far = 3000.0;
//------
ObjManager obj_manager;
//-----static grobal value
bool meshHolder::enable_picker;
int meshHolder::nearlest_vertex_from_picker;
vector<meshContainer> meshHolder::mesh_imported;




//--------------------------------------------------------------
void ofApp::setup(){
    //---realsense
    real_sense.setup();
    //g-buffer
    geeBuffer.setup(480, 400, 480, 270);
    geeBuffer.set_camera_param(70);
    //-----
    viewer.setup(0 ,0 ,480, 270);
    //------imGUI セットアップ
    gui.setup();
    //--video capture
    uvc_cap.get_camera_list();
    //3d perspective
    perspective.allocate(area2.getWidth(), area2.getHeight());
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
   //--gizmo
    gizmocam.setControlArea(area2);
    
}
//--------------------------------------------------------------
void ofApp::update(){
    UVCimage.setFromPixels(uvc_cap.pixels);
    glm::mat4 m_ = eigen_glm(real_sense.ultrasound);
    viewer.update(UVCimage, m_);
    //gizomo rotation model matrix
    mygizmo.unable_easycam(easycam);
    mygizmo.update(gizmocam);
    
    drawTerrios(perspective, easycam);

    
    
    perspective.begin();{
        easycam.begin();{
            //mvm_easycam = easycam.getModelViewMatrix();
            easycam.setNearClip(near);
            easycam.setFarClip(far);
            ofPushMatrix();{
                for(auto mp: viewer.stacks){
                    ofPushMatrix();
                    ofMultMatrix(mp.mat);
                
                    mp.fbo.getTexture().bind();
                    mp.mesh.draw();
                    mp.fbo.getTexture().unbind();
                    ofPopMatrix();
                }
                ofEnableSeparateSpecularLight();
                ofLight light;
                light.enable();
                for(int i = 0; i < obj_manager.obj_models.size(); i++){
                    if(obj_manager.obj_models[i].show){
                        ofSetColor(ofColor::gray);
                        //obj_manager.obj_models[i].model.drawWireframe();
                        
                        glPointSize(2);
                        ofSetColor(ofColor::red);
                        //obj_manager.obj_models[i].model.drawVertices();
                        obj_manager.obj_models[i].model.drawFaces();
                    }
                }
                ofMultMatrix(mygizmo.model);
                //ofTranslate(20, 20, -20);
                //ofDrawBox(40);
            }ofPopMatrix();
        }easycam.end();
    }perspective.end();
    
    fisheye_left_image.setFromPixels(real_sense.colorimg.ptr(), 848, 800, OF_IMAGE_COLOR);
    
    draw_model(perspective, easycam, realsense_model, real_sense.t265_rawoutput, ofColor(200));
    draw_axis(perspective, easycam, real_sense.ultrasound);
    //draw_axis(perspective, easycam, real_sense.scope);
    if(real_sense.tvecs_.size()>0){
        //std::cout<<tst->tvecs_[0]<<std::endl;
    }
    arucoManager.update( real_sense.markerIdsL_, real_sense.tvecs_, real_sense.rvecs_, real_sense.markerCornersL_, real_sense.t265_rawoutput);
    arucoManager.draw(perspective, easycam);
    vector<MeshConatiner> container_stack;
    for( auto ctn : viewer.stacks){
        container_stack.push_back(MeshConatiner{ctn.mesh, ctn.mat, ctn.fbo, glm::vec4(0,0,0,0), true});
    }
    glm::mat4 mvm_easycam = easycam.getModelViewMatrix();
    glm::mat4 mvm = glm::inverse(mvm_easycam);
    
    geeBuffer.update(mvm, container_stack);
    geeBuffer.pick(mouseX, mouseY, mvm);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDisableDepthTest();
    glDisable(GL_LIGHTING);
    fisheye_left_image.draw(480,0,424,400);
    perspective.draw(area2);
    geeBuffer.fbo.draw(480,400);
    viewer.fbo.draw(viewer.px,  viewer.py, viewer.width, viewer.height);
    //viewer.stacks[viewer.stacks.size() - 1].fbo.draw(0,0);
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
    uvc_cap.stopThread();
    real_sense.disconnect();
}
//--------------------------------------------------------------
void ofApp::gui_draw(){
    gui.begin();{
        ImGui::Begin("ultrasound");{
            if(ImGui::IsWindowHovered()){
                easycam.disableMouseInput();
            }
            //ImGui::Checkbox("flip", &isEnable);//https://qiita.com/Ushio/items/446d78c881334919e156
            static char buf[256] = "";//http://www.sanko-shoko.net/note.php?id=qlv3
            if (ImGui::InputText("markup", buf, 256)) {
               
            }
            
            ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, (ImVec4)ImColor(255,255,255,255));
            ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(viewer.hue , 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(viewer.hue , 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(viewer.hue , 1.0f, 1.0f));
            //if(ImGui::SliderFloat("HUE", &viewer.hue, 0.0f, 1.0f)){
            if(ImGui::SliderFloat("HUE", &viewer.hue, 0.0f, 1.0f)){
                viewer.change_color();
            };
            ImGui::PopStyleColor(4);
            //ofColor col = ofColor::fromHsb(viewer._hue * 256,255,255 );
            //ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, (ImVec4)ImColor(255,255,255,255));
            if(ImGui::SliderFloat("RADIUS", &viewer.radius, 1.00f, 20.0f)){
                viewer.change_radius();
            };
            if (ImGui::Button(" delete ")) {
                viewer.delete_marker();

            }
            ImGui::SameLine();
            if (ImGui::Button("delete all ")) {
                viewer.delete_all_marker();
            }
            if (ImGui::Button(" capture! ")) {
                viewer.capture(UVCimage);
            }
            
            if (ImGui::Button(" delete image ")) {
                //----------未実装
            }
            if (ImGui::Button("<")) {
                viewer.backward_page();
            }
            ImGui::SameLine();if (ImGui::Button(">")) {
                viewer.forward_page();
            }
            //ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, (ImVec4)ImColor(255,255,255,255));v
            if(ImGui::SliderInt("page", &viewer.current_page, 0, viewer.stacks.size() -1)){
                viewer.change_page();
                
            }
            if(ImGui::SliderFloat("threshold", &viewer.threshold, 0.00f, 600.0f)){
                viewer.update_threshold();
            }
            ImGui::Text(" gauge :");
            ImGui::SameLine();
            if (ImGui::Button(" reset ")) {
                //----------未実装
            }
            ImGui::SameLine();
            if (ImGui::Button(" save ")) {
                //----------未実装
            }
            ImTextureID textureID = ( ImTextureID )( uintptr_t )viewer.preview.getTexture().getTextureData().textureID ;
            ImGui::Image( textureID, ImVec2(viewer.preview.getWidth(),viewer.preview.getHeight()) ) ;
        }ImGui::End();
        ImGui::Begin("UVC source");{
            if(ImGui::IsWindowHovered()){
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
                if (ImGui::Button(" stop camera ")) {
                    uvc_cap.stop_cam();
                }
            }
            ImGui::Text("Hello");
        }ImGui::End();
        ImGui::Begin("hello realsense");{
            if(ImGui::IsWindowHovered()){
                //viewer.hover = true; //
                easycam.disableMouseInput();
            }
            if (ImGui::Button(" connect ")) {
                if(real_sense.connect()){
                    real_sense.startThread();
                };
            }
            if (ImGui::Button(" disconnect ")) {
                real_sense.disconnect();
            }
            ImGui::SliderFloat("process", &real_sense.iProcessCov, 0.00f, 200.0f);
            ImGui::SliderFloat("measure", &real_sense.iMeasurementCov, 0.00f, 200.0f);
            if (ImGui::Button(" CAPTURE ")) {
                
            }
            if (ImGui::Button(" STOP CAPTURE ")) {
                
            }
            if (ImGui::Button(" ADJUST ")) {
                
            }
            if (ImGui::Button(" STOP ADJUST ")) {
                
            }
        }ImGui::End();
        ImGui::Begin("read CT model");{
            if(ImGui::IsWindowHovered()){
                //viewer.hover = true;
                easycam.disableMouseInput();
            }
            
            if (ImGui::Button(" open obj file ")) {
                ofFileDialogResult result = ofSystemLoadDialog("Load file");
                if(result.bSuccess) {
         
                    string path = result.getPath();
                    //ofxAssimpModelLoader model_segmentaion;
                    objLoader o = objLoader(path);
                    if(o.set){
                        obj_manager.obj_models.push_back(o);
                    }
                    glm::mat4 E(1.0f);
                    glm::vec4 color_= glm::vec4( 1.0, 0.0, 0.0 , 1.0);
                    for( auto ml : o.mesh_){
                        //geeBuffer.container_manager.add_mesh(ml, o.thum);
                        mesh_controller.add_mesh(ml, o.thum);
                    }
                    
                }
            }
            if(obj_manager.available()){
                if(obj_manager.show()){
                    if (ImGui::Button(" hide ")) {
                        obj_manager.flip_show();
                    }
                }else{
                    if (ImGui::Button(" show ")) {
                        obj_manager.flip_show();
                    }
                }
                string c_ = obj_manager.getPath();
                char* c = const_cast<char*>(c_.c_str());
                
                ImGui::SameLine();ImGui::Text(c);
                
                if (ImGui::Button("<")) {
                    obj_manager.backward_page();
                }
                ImGui::SameLine();if (ImGui::Button(">")) {
                    obj_manager.forward_page();
                }
                ImGui::SameLine();if (ImGui::Button(" delete obj ")) {
                    obj_manager.delete_obj();
                }
            }
            
            
        }ImGui::End();
       
        ImGui::Begin("endoscope");{
            if(ImGui::IsWindowHovered()){
                //viewer.hover = true; // GUI上にマウスがあるときにcropウインドウの操作をキャンセルさせるため
                easycam.disableMouseInput();
            }
            
            if (ImGui::Button("on")) {
               // g_buf.endoscope = true;
            }
            if (ImGui::Button("g-buffer")) {
                //easycamEnable = !easycamEnable;
            }
           
            if(ImGui::SliderFloat("Float", &fov, 40.0f, 120.0f)){
                easycam.setFov(fov);
                geeBuffer.set_camera_param(fov);
            }
            if(ImGui::SliderFloat("Near", &near, 1.0f, 200.0f)){
                easycam.setFov(fov);
            }
            if(ImGui::SliderFloat("far", &far, 500.0f, 5000.0f)){
                easycam.setFov(fov);
            }
        }ImGui::End();
        ImGui::Begin("mesh profile");{
            if(ImGui::IsWindowHovered()){
                //viewer.hover = true;
                easycam.disableMouseInput();
            }
            if(geeBuffer.container_manager.available()){
                ImGui::Text("group : ");
                ImGui::SameLine();string c1_ = geeBuffer.container_manager.getParent();
                char* c1 = const_cast<char*>(c1_.c_str());
                
                ImGui::Text(c1);
                
                string c2_ = to_string(geeBuffer.container_manager.current_page);
                char* c2 = const_cast<char*>(c2_.c_str());
                ImGui::SameLine();ImGui::Text(c2);
                ImGui::SameLine();
                if(geeBuffer.container_manager.show()){
                    if (ImGui::Button(" hide ")) {
                        geeBuffer.container_manager.flip_show();
                    }
                }else{
                        if (ImGui::Button(" show ")) {
                            geeBuffer.container_manager.flip_show();
                        }
                }
                
                static char buf1[256] = "";//http://www.sanko-shoko.net/note.php?id=qlv3
                if (ImGui::InputText("name", buf1, 256)) {
                   
                }
  
           
                if(ImGui::Checkbox(" pick target ", &geeBuffer.container_manager.enable_picker)){
                    geeBuffer.container_manager.enable_pick();
                }
                if(ImGui::SliderInt("PORT ID", &geeBuffer.port_id, 0, 2)){
                    
                };
            
                if(ImGui::ColorEdit3("color", geeBuffer.container_manager.col1)){
                    geeBuffer.container_manager.change_color();
                }
                /*
                if(ImGui::SliderFloat("HUE2", &geeBuffer.container_manager.hue, 0.0f, 1.0f)){
                    geeBuffer.container_manager.change_color();
                };
                 */
                if (ImGui::Button(" apply group ")) {
                    geeBuffer.container_manager.change_color_group();
                }
                if (ImGui::Button("<")) {
                    geeBuffer.container_manager.backward_page();
                }
                ImGui::SameLine();if (ImGui::Button(">")) {
                    geeBuffer.container_manager.forward_page();
                }
                ImGui::SameLine();if (ImGui::Button(" delete mesh ")) {
                    //obj_manager.delete_obj();
                }
                
            }
        }ImGui::End();
    }gui.end();
 
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if( key == 8 ){
        viewer.delete_marker();
    }
    mygizmo.enable_gozmo(gizmocam);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
 
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
   
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
/*
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
*/
/*
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
                for(auto &s:image_stacker.stkimgs){
                    ofPushMatrix();{
                        ofMultMatrix(eigen_glm(s.mat));
                        ofDisableArbTex();
                        geo_shader.setUniformTexture("image", s.fbo.getTexture(), 0);
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
*/
/*
ofMesh m;
m = rectMesh(0,0,100,100,true);
setNormals(m);

/*
//-----generate saple sceen
Eigen::Matrix4d M;
M.setIdentity();
for(int i = 0; i < 10; i++){
    ofFbo f;
    ofDisableArbTex();
    f.allocate(256,256);
    f.begin();
    ofClear(0,0);
    for(int k = 0; k < 10; k++)
    {
        ofSetColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255),ofRandom(200,255));
        ofDrawCircle(ofRandom(0,255), ofRandom(0,255), 10);
    }
    f.end();
    
    M = rotx( i * 30);
    glm::mat4 MM = eigen_glm(M);
    ofMesh msh;
    msh = rectMesh(0,0,100,100,true);
    setNormals(msh);
    MeshConatiner mct = MeshConatiner{msh, MM, f, glm::vec4(0,0,0,0), true};
    mesh_container_stack.push_back(mct);
}
*/
