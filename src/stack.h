#pragma once
#include <iostream>
#include <vector>
#include "ofApp.h"
#include "sheared_stracture.h"
namespace satackviewer

{
class object_{
    public:
    float px,py;
    int hue;
    bool selected;
    float radius;
    bool focus;
    bool onDrag;
    bool updated;
    float start_px, start_py;
    float strat_mouse_x, start_mouse_y;
    int type;
 
    object_() = default;//https://www.delftstack.com/ja/howto/cpp/array-of-objects-in-cpp/
    object_(float _x, float _y, int _hue, float _radius):px(_x), py(_y), hue(_hue), radius(_radius){
        type = 0;
        selected = true;
    }
    object_(float _x, float _y, int _hue, float _radius, int _type):px(_x), py(_y), hue(_hue), radius(_radius), type(_type){
        selected = true;
    }
    virtual ~object_() = default;
    bool checkFocus(glm::vec2 vec){
        glm::vec2 len(vec.x - px, vec.y - py);
        return glm::length(len) < radius;
    }
    bool mouse_release(glm::vec2 vec){
        onDrag = false;
    }
    bool mouse_release(){
        onDrag = false;
    }
    bool mouse_move(glm::vec2 vec){
        glm::vec2 len(vec.x - px, vec.y - py);
        focus =  glm::length(len) < radius;
        onDrag = false;
        return focus;
    }
    bool mouse_move(float x, float y){
        glm::vec2 len(x - px, y - py);
        focus =  glm::length(len) < radius;
        onDrag = false;
        return focus;
    }
    void mouse_drag(glm::vec2 vec){
        if(onDrag){
            px = start_px + vec.x - strat_mouse_x;
            py = start_py + vec.y - start_mouse_y;
            selected = true;
            updated = true;
        }
    }
    void mouse_drag(float x, float y){
        if(onDrag){
            px = start_px + x - strat_mouse_x;
            py = start_py + y - start_mouse_y;
            selected = true;
            updated = true;
        }
    }
    bool mouse_press(glm::vec2 vec){
        glm::vec2 len(vec.x - px, vec.y - py);
        bool check = glm::length(len) < radius;
        if(check){
            
            strat_mouse_x = vec.x;
            start_mouse_y = vec.y;
            start_px = px;
            start_py = py;
                selected = true;
   
            onDrag = true;
        }
        return check;
    }
    bool mouse_press(float x, float y){
        glm::vec2 len(x - px, y - py);
        bool check = glm::length(len) < radius;
        if(check){
            strat_mouse_x = x;
            start_mouse_y = y;
            start_px = px;
            start_py = py;
                selected = true;
   
            onDrag = true;
        }
        return check;
    }
    virtual void draw(){
            ofSetColor(ofColor::fromHsb(hue, 255, 255));
        if(focus){
            ofSetColor(ofColor::fromHsb(hue, 255, 100));
        }
        
            ofDrawCircle(px, py, radius);
        if(selected){
            ofSetColor(220);
            ofNoFill();
            ofDrawCircle(px, py, radius + 1);
            ofFill();
        }
       
    }
    virtual void draw_holizontal_line(int width){
        px = width - 10;
        ofSetColor(100);
        ofDrawLine(0, py, width,py);
        ofSetColor(ofColor::fromHsb(hue, 255, 255));//Hue,Saturation,Brightness
        ofDrawCircle(px, py, radius);
    }
    virtual void draw_vertical_line(int height){
        py = 10;
        ofSetColor(100);
        ofDrawLine(px, 0, px, height);
        ofSetColor(ofColor::fromHsb(hue, 255, 255));//Hue,Saturation,Brightness
        ofDrawCircle(px, py, radius);
    }
    virtual void draw(int width, int height){
    }
    void set_param(float _x, float _y, int _hue, float _radius){
        px = _x; py = _y; hue = _hue; radius = _radius;
    }
};


class stack{
public:
    int width, height;
    //-------------
    bool show;
    bool select;
    string fileName;
    ofImage image;

    //----------
    std::vector<object_> rectangle_;
    std::vector<object_> marker;
    std::vector<object_> gauge;
    
    float threshold;
    bool crop_flag;
    
    float hue;
    float radius;
    //-----
    ofMesh mesh;
    bool use_tecxture;
    ofFbo fbo;
    glm::mat4 mat;
    glm::vec4 color;
    
    //-------
    stack(int _width, int _height):width(_width), height(_height){
        rectangle_.push_back(object_());
        rectangle_.push_back(object_());
        rectangle_.push_back(object_());
        rectangle_.push_back(object_());
        gauge.push_back(object_(width - 20, 20, 40,  6));
        gauge.push_back(object_(width - 20, 60, 40,  6));
        gauge.push_back(object_(width / 2.0,10, 100, 6));
        reset_gauge();
    }
    stack(){
        
    }
    void mouse(int x, int y, int button, int function, float _hue, float _radius){
            switch (function) {
                case 0://mouseMove
                    for(auto &g :gauge){
                        g.focus = false;
                        g.mouse_move(x,y);
                    }
                    for(auto &a :rectangle_){
                        a.focus = false;
                        a.mouse_move(x,y);
                    }
                    for(auto &p :marker){
                        p.focus = false;
                        p.mouse_move(x,y);
                    }
                    break;
                case 2://mouse_press
                    if(button == 0){
                        for(auto &g :gauge){
                            g.selected = false;
                            g.mouse_press(x,y);
                        }
                        for(auto &a :rectangle_){
                            a.selected = false;
                            a.mouse_press(x,y);
                        }
                        for(auto &p :marker){
                            p.selected = false;
                            p.mouse_press(x,y);
                        }
                    }
                    break;
                case 3://mouse_releas
                    if(button == 2){
                        for(auto &g :gauge){
                            g.selected = false;
                        }
                        for(auto &a :rectangle_){
                            a.selected = false;
                        }
                        for(auto &p :marker){
                            p.selected = false;
                        }
                       
                        marker.push_back(object_(x, y, _hue, _radius));
                      
                    }else if(button == 0) {
                        for(auto &g :gauge){
                            g.mouse_release();
                        }
                        for(auto &a :rectangle_){
                            a.mouse_release();
                        }
                        for(auto &p :marker){
                            p.mouse_release();
                        }
                    }
                    break;
                case 1://mouse_drag
                    if(button == 0){
                        for(auto &g :gauge){
                            g.mouse_drag(x,y);
                        }
                        for(auto &a :rectangle_){
                            a.mouse_drag(x,y);
                        }
                        for(auto &p :marker){
                            p.mouse_drag(x,y);
                        }
                    }
                    break;
                default:
                    break;
            }
    }
    
    void update(ofFbo &fbo){
        fbo.begin();{
            //ofClear(0,0);
            gauge[0].draw_holizontal_line(width);
            gauge[1].draw_holizontal_line(width);
            gauge[2].draw_vertical_line(height);
            if(rectangle_[0].updated){
                if(rectangle_[0].px  > rectangle_[1].px - 40 ){
                    rectangle_[0].px = rectangle_[1].px - 40;
                }
                if(rectangle_[0].py  > rectangle_[3].py - 40 ){
                    rectangle_[0].py = rectangle_[3].py - 40;
                }
                rectangle_[1].py = rectangle_[0].py;
                rectangle_[3].px = rectangle_[0].px;
            }
            if(rectangle_[1].updated){
                if(rectangle_[1].px  < rectangle_[0].px + 40 ){
                    rectangle_[1].px = rectangle_[0].px + 40;
                }
                if(rectangle_[1].py  > rectangle_[2].py - 40 ){
                    rectangle_[1].py = rectangle_[2].py - 40;
                }
         
                rectangle_[0].py = rectangle_[1].py;
                rectangle_[2].px = rectangle_[1].px;
            }
            if(rectangle_[2].updated){
                if(rectangle_[2].px  < rectangle_[3].px + 40 ){
                    rectangle_[2].px = rectangle_[3].px + 40;
                }
                if(rectangle_[2].py  < rectangle_[1].py + 40 ){
                    rectangle_[2].py = rectangle_[1].py + 40;
                }
                rectangle_[3].py = rectangle_[2].py;
                rectangle_[1].px = rectangle_[2].px;
            }
            if(rectangle_[3].updated){
                if(rectangle_[3].px  > rectangle_[2].px - 40 ){
                    rectangle_[3].px = rectangle_[2].px - 40;
                }
                if(rectangle_[3].py  < rectangle_[1].py + 40 ){
                    rectangle_[3].py = rectangle_[1].py + 40;
                }
                rectangle_[2].py = rectangle_[3].py;
                rectangle_[0].px = rectangle_[3].px;
            }
            
   
            ofSetColor(ofColor::fromHsb(200, 255, 255));
            ofDrawLine(rectangle_[0].px, rectangle_[0].py,  rectangle_[1].px, rectangle_[1].py);
            ofDrawLine(rectangle_[1].px, rectangle_[1].py,  rectangle_[2].px, rectangle_[2].py);
            ofDrawLine(rectangle_[2].px, rectangle_[2].py,  rectangle_[3].px, rectangle_[3].py);
            ofDrawLine(rectangle_[3].px, rectangle_[3].py,  rectangle_[3].px, rectangle_[1].py);
            crop_flag = false;
            for(auto &a :rectangle_){
                if(a.updated){
                    crop_flag = true;
                };
            }
            for(auto &a :gauge){
                if(a.updated){
                    crop_flag = true;
                };
            }
            for(auto &a :rectangle_){
                a.draw();
                a.updated = false;
            }
            for(auto &a :marker){
                a.draw();
                //a.updated = false;
            }
            
        }fbo.end();
        
    }
    void get_radius(float &r){
        for(auto &a :marker){
            if(a.selected){
                r = a.radius;
            }
        }
    }
    void get_hue(float &c){
        for(auto &a :marker){
            if(a.selected){
                c = a.hue / 256.0;
            }
        }
    }
    void change_radius(float r){
        for(auto &a :marker){
            if(a.selected){
                a.radius = r;
            }
        }
    }
    void change_color(float h){
        for(auto &a :marker){
            if(a.selected){
                a.hue = h * 256.0;
            }
        }
    }
    void delete_marker(){
        for(int i = 0; i < marker.size(); i++){
            if(marker[i].selected){
                marker.erase(marker.begin() + i, marker.begin() + i + 1);
            }
        }
    }
    void delete_all_marker(){
        marker.clear();
    }
    void set_from_json(ofJson &js){
        gauge[0].py = js["pt"][0][0];
        gauge[1].py = js["pt"][0][1];
        gauge[2].px = js["pt"][0][2];
        rectangle_[0].px = js["pt"][0][3];
        rectangle_[0].py = js["pt"][0][4];
        rectangle_[2].px = js["pt"][0][5];
        rectangle_[2].py = js["pt"][0][6];
        rectangle_[1].px = rectangle_[2].px;
        rectangle_[1].py = rectangle_[0].py;
        rectangle_[3].px = rectangle_[0].px;
        rectangle_[3].py = rectangle_[2].py;
    }

    void reset_gauge(){
        gauge[0].set_param(width - 20, 20, 40,  6);
        gauge[1].set_param(width - 20, 60, 40,  6);
        gauge[2].set_param(width / 2.0,10, 100, 6);
        rectangle_[0].set_param(10,10,                  60,6);
        rectangle_[1].set_param(width - 20, 10,         60,6);
        rectangle_[2].set_param(width - 20,height - 10, 60,6);
        rectangle_[3].set_param(10,height - 10,         60,6);
    }
    void save_gauge(ofJson &j){
        ofJson pt;
        pt[0] = gauge[0].py;
        pt[1] = gauge[1].py;
        pt[2] = gauge[2].px;
        pt[3] = rectangle_[0].px;
        pt[4] = rectangle_[0].py;
        pt[5] = rectangle_[2].px;
        pt[6] = rectangle_[2].py;
        j["pt"].push_back(pt);
    }
    void crop(ofImage &img){
        float crop_width_on_viewer = rectangle_[1].px - rectangle_[0].px;
        float crop_height_on_viewer = rectangle_[3].py - rectangle_[0].py;
        float mm_pixel = 10.0 / abs(gauge[1].py -  gauge[0].py );  // 10mm =  mm_pixel
        float crop_width = crop_width_on_viewer / width *  img.getWidth();
        float crop_hight = crop_height_on_viewer / height *  img.getHeight();
        float crop_px = rectangle_[0].px / width *  img.getWidth();
        float crop_py = rectangle_[0].py / height *  img.getHeight();
        float mesh_real_space_width = crop_width * mm_pixel;
        float mesh_real_space_height = crop_hight * mm_pixel;
        
        ofFbo f;
        ofDisableArbTex();
        f.allocate(crop_width,crop_hight);
        f.begin();
        ofClear(0,0);
        img.draw(-crop_px, -crop_py);
        f.end();
        ofDisableArbTex();
        fbo.allocate(mesh_real_space_width, mesh_real_space_height);
        fbo.begin();
        ofClear(0,0);
        f.draw(0,0, mesh_real_space_width, mesh_real_space_height);
        fbo.end();
        //---
        ofPixels pixels;
        fbo.readToPixels(pixels);
        for(int x = 0; x < pixels.getWidth(); x++ ){
            for(int y = 0; y < pixels.getHeight(); y++ ){
                ofColor c = pixels.getColor(x,y);
                if( c.r + c.g + c.b < this->threshold){
                    ofColor c = ofColor(0,0,0,0);
                    pixels.setColor(x, y, c);
                }
            }
        }
        ofImage i;
        i.setFromPixels(pixels);
        fbo.begin();
        ofClear(0,0);
        i.draw(0,0);
        fbo.end();
        this->mesh = rectMesh(0,0,mesh_real_space_width,mesh_real_space_height,true);
        setNormals(this->mesh);
    }
    void simple_crop(ofImage &img){
        float crop_width_on_viewer = rectangle_[1].px - rectangle_[0].px;
        float crop_height_on_viewer = rectangle_[3].py - rectangle_[0].py;
        float mm_pixel = 10.0 / abs(gauge[1].py -  gauge[0].py );  // 10mm =  mm_pixel
        float crop_width = crop_width_on_viewer / width *  img.getWidth();
        float crop_hight = crop_height_on_viewer / height *  img.getHeight();
        float crop_px = rectangle_[0].px / width *  img.getWidth();
        float crop_py = rectangle_[0].py / height *  img.getHeight();
        float mesh_real_space_width = crop_width * mm_pixel;
        float mesh_real_space_height = crop_hight * mm_pixel;
        
        ofFbo f;
        ofDisableArbTex();
        f.allocate(crop_width,crop_hight);
        f.begin();
        ofClear(0,0);
        img.draw(-crop_px, -crop_py);
        f.end();
        ofDisableArbTex();
        fbo.allocate(mesh_real_space_width, mesh_real_space_height);
        fbo.begin();
        ofClear(0,0);
        f.draw(0,0, mesh_real_space_width, mesh_real_space_height);
        fbo.end();
        this->mesh = rectMesh(0,0,mesh_real_space_width,mesh_real_space_height,true);
        setNormals(this->mesh);
    }
};
//-------------------------------------------------//

class manager{
    bool inner(float x, float y){
        return  ( px < x && x < ( px + width ) && py < y && y < ( py + height ));
    }
    glm::vec2 get_viwer_position(float x, float y){
        float X = x - px;
        float Y = y - py;
        return glm::vec2(X, Y);
    }
    glm::vec2 get_screen_position(float x, float y){
        float X = x + px;
        float Y = y + py;
        return glm::vec2(X, Y);
    }
    
    
    int snapCounter;
    
    //----
    ofJson js;
    
public:
    int current_page;
    vector<stack>stacks;
    float px, py;//表示位置
    float width, height; // 表示サイズ
    float hue = 0.5;
    float radius = 6;
    float threshold = 200;
    ofImage noimage;
    ofImage captured;
    ofFbo fbo;
    ofFbo preview;
    ofFbo live_preview;
    //float preview_offset_x, preview_offset_y;
    
 
    manager(){
        
    }

    void setup(float _x, float _y, float _w, float _h){
        px = _x;
        py = _y;
        width = _w;
        height = _h;
        try{
            ofFile file("us_viewer_setting.json");
            file >> js;
 
        std::cout<<"read us setting file2 "<<std::endl;
        }
        catch(...)
        {
            std::cout<<"cannot read file"<<std::endl;
        }
        fbo.allocate(width, height);
        add_stack();
        change_page();
        noimage.load("chessboard1.jpg");
  
            ofRegisterMouseEvents(this); // this will enable our circle class to listen to the mouse events.
           
       
    }
    void mouseMoved(ofMouseEventArgs & args){
    }
    void mouseDragged(ofMouseEventArgs & args){};
    void mousePressed(ofMouseEventArgs & args){};
    void mouseReleased(ofMouseEventArgs & args){};
    void mouseScrolled(ofMouseEventArgs & args){};
    void mouseEntered(ofMouseEventArgs & args){};
    void mouseExited(ofMouseEventArgs & args){};
    
    void capture(ofImage &image){
        string fileName = "snapshot/snapshot_"+ofToString(snapCounter, 5, '0')+".png";
        if(image.save(fileName)){
            
            stacks.push_back( stacks[ stacks.size() - 1 ] );
 
            stacks[ stacks.size() - 2 ].fileName = fileName;
            //stacks[ stacks.size() - 2 ].mat = mat;
            stacks[ stacks.size() - 2 ].threshold = this->threshold;
            stacks[ stacks.size() - 2 ].crop(image);
            snapCounter ++;
            current_page = stacks.size() - 1;
            cout<<current_page<<" "<<stacks.size()<<endl;
        }else{
            std::cout<<"err"<<std::endl;
        };
    }
    void change_page(){
        if(current_page != stacks.size() -1){
            captured.load(stacks[current_page].fileName);
            this->threshold = stacks[current_page].threshold;
        }
    }
    
    void add_stack(){
        stack s(width, height);
        s.set_from_json(js);
        //s.mat = glm::mat4(0);
        stacks.push_back(s);
        current_page = stacks.size() - 1;
        change_page();
    }
    void delete_stack(){
        if(stacks.size() > 0){
            stacks.erase(stacks.begin() + current_page, stacks.begin() + current_page + 1);
            current_page --;
            if( current_page < 0){
                current_page = 1;
            }
        }
        change_page();
    }
    void forward_page(){
        current_page ++;
        if(current_page > stacks.size() - 1){
            current_page = stacks.size() - 1;
        }
        change_page();
        cout<<current_page<<" "<<stacks.size()<<endl;
    }
    void backward_page(){
        current_page --;
        if( current_page < 0){
            current_page = 0;
        }
        change_page();
        cout<<current_page<<" "<<stacks.size()<<endl;
    }
    void mouse(int x, int y, int button, int function){
        if(inner(x,y)){
            glm::vec2 vec;
            vec = get_viwer_position(x,y);
            float hue_ = hue * 256;
            stacks[current_page].mouse(vec.x, vec.y, button, function, hue_, radius);
        }
    }
    void update(ofImage &uvc_image, glm::mat4 mat){
        if(current_page != stacks.size() -1){
            fbo.begin();
            ofClear(0,0);
            captured.draw(0,0, width, height);
            fbo.end();
            stacks[current_page].update(fbo);
            if( stacks[current_page].crop_flag ){
                stacks[current_page].crop(captured);
            }
        }else{
            fbo.begin();
            ofClear(0,0);
            uvc_image.draw(0,0, width, height);
            fbo.end();
            stacks[current_page].update(fbo);
            stacks[current_page].simple_crop(uvc_image);
        }
        stacks[stacks.size() -1].mat = mat;
        stacks[current_page].get_radius(radius);
        stacks[current_page].get_hue(hue);
        //preview = stacks[stacks.size() -1].fbo;//本当に大丈夫！？？
        preview = stacks[current_page].fbo;
        live_preview = stacks[stacks.size() - 1].fbo;
    }
    void change_radius(){
        stacks[current_page].change_radius(radius);
    }
    void change_color(){
        stacks[current_page].change_color(hue);
    }
    void delete_all_marker(){
        stacks[current_page].delete_all_marker();
    }
    void delete_marker(){
        stacks[current_page].delete_marker();
    }
    void exit(){
        ofJson j;
        if(stacks.size() > 0){
            stacks[current_page].save_gauge(j);
            ofSaveJson("us_viewer_setting.json", j);
        }else{
            cout << "save json err" << endl;
        }
    }
    void reset_gauge(){
        
    }
    void crop(){
        if(current_page != stacks.size() -1){
            stacks[current_page].crop(captured);
        }
    }
    void update_threshold(){
        if(current_page != stacks.size() -1){
            stacks[current_page].threshold = this->threshold;
            stacks[current_page].crop(captured);
        }
    }
};
};
