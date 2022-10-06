#include "ofApp.h"
Viewer viewer(110 ,50 ,320,240);
//--------------------------------------------------------------
void ofApp::setup(){
    viewer.fbo_allocate(fbo);
    //------imGUI セットアップ
    gui.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    viewer.update(fbo);
    viewer.hover = false;
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.draw(viewer.px,  viewer.py, viewer.width, viewer.height);
    gui.begin();{
        ImGui::Begin("ultrasound");{
            if(ImGui::IsWindowHovered()){
                viewer.hover = true;
                //easycam.disableMouseInput();
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
        /*
        ImGui::Begin("UVC source");{
            if(ImGui::IsWindowHovered()){
                gui_hover = true;// GUI上にマウスがあるときにcropウインドウの操作をキャンセルさせるため
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
                uvc_cap.set_param(listbox_item_current,1280,720);
                if(!uvc_cap.isThreadRunning()){
                    uvc_cap.startThread();
                }
            }
            ImGui::Text("Hello");
        }ImGui::End();
         */
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
