#pragma once
#include <memory>
#include "ofApp.h"
#include "sheared_stracture.h"

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
        }
    }
    void mouse_drag(float x, float y){
        if(onDrag){
            px = start_px + x - strat_mouse_x;
            py = start_py + y - start_mouse_y;
            selected = true;
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
    virtual void draw(ofFbo &fbo){
        fbo.begin();
            ofSetColor(ofColor::fromHsb(hue, 255, 255));
            ofDrawCircle(px, py, radius);
        fbo.end();
    }
    void set_param(float _x, float _y, int _hue, float _radius){
        px = _x; py = _y; hue = _hue; radius = _radius;
    }
};
class Holizontal : public object_ {
    void draw(ofFbo &fbo){
        fbo.begin();
        px = fbo.getWidth() - 10;
            ofSetColor(100);
            ofDrawLine(0, py, fbo.getWidth(),py);
            ofSetColor(ofColor::fromHsb(hue, 255, 255));//Hue,Saturation,Brightness
            ofDrawCircle(px, py, radius);
        fbo.end();
    }
};
class Vertical : public object_ {
    void draw(ofFbo &fbo){
        fbo.begin();
        py = 10;
            ofSetColor(100);
            ofDrawLine(px, 0, px, fbo.getHeight());
            ofSetColor(ofColor::fromHsb(hue, 255, 255));//Hue,Saturation,Brightness
            ofDrawCircle(px, py, radius);
        fbo.end();
    }
};


class Manager_{
    int currne_page = 0;
    int num_sceen = 0;
    Manager_(){
        
    }
    void add_sceen(){
        
    }
    void delete_sceen(){
        
    }
    void clear_all_sceen(){
        
    }
    void foward(){
        
    }
    void backward(){
        
    }
};

class Viewer{
public:
    bool hover;
    float px, py;
    float width, height;
    //float image_width, image_height; //もしかしたら不要かもしれない
    std::vector<object_> rectangle_;
    std::vector<std::shared_ptr<object_> > gause;
    Viewer(float _x, float _y, float _w, float _h):px(_x), py(_y), width(_w), height(_h){
        //https://qiita.com/MusicScience37/items/1ba81db1592c974f0632
        auto d1 = std::make_shared<Holizontal>();
        auto d2 = std::make_shared<Holizontal>();
        auto h1 = std::make_shared<Vertical>();
        gause.push_back(d1);
        gause.push_back(d2);
        gause.push_back(h1);
        gause[0]->set_param(width - 20, 20, 40,  6);
        gause[1]->set_param(width - 20, 60, 40,  6);
        gause[2]->set_param(width / 2.0,10, 100, 6);
        rectangle_.push_back(object_(10,10,                  60,6));
        rectangle_.push_back(object_(width - 20, 10,         60,6));
        rectangle_.push_back(object_(width - 20,height - 10, 60,6));
        rectangle_.push_back(object_(10,height - 10,         60,6));
    }
    Viewer(){
    }
    void fbo_allocate(ofFbo &fbo){
        fbo.allocate(width, height);
    }
    void setImageSize(float w, float h){
        
    }
    void setStack(){
        
    }
    void getMarkers(){
        
    }
    void update(ofFbo &fbo){
        fbo.begin();{
            ofClear(0);
        }fbo.end();
        for(int i = 0; i < 3; i++){
            gause[i]->draw(fbo);
        }

        for(auto &a :rectangle_){
            a.draw(fbo);
            a.updated = false;
        }
    }
    /*
    void draw(ofFbo &fbo){
        for(int i = 0; i < 3; i++){
            gause[i]->draw(fbo);
        }
        for(auto a :rectangle_){
            a.draw(fbo);
        }
    }
     */
    void mouse(int x, int y, int button, int function){
        if(inner( x, y )){
            switch (function) {
                case 0://mouseMove
                    for(int i = 0; i < 3; i++){
                        gause[i]->focus = false;
                        gause[i]->mouse_move(get_viwer_position(x,y));
                    }
                    for(auto &a :rectangle_){
                        a.focus = false;
                        a.mouse_move(get_viwer_position(x,y));
                    }
                    break;
                case 2://mouse_press
                    if(button == 0){
                        for(int i = 0; i < 3; i++){
                            gause[i]->selected = false;
                            gause[i]->mouse_press(get_viwer_position(x,y));
                        }
                        for(auto &a :rectangle_){
                            a.selected = false;
                            a.mouse_press(get_viwer_position(x,y));
                        }
                    }
                    break;
                case 3://mouse_releas
                    if(button == 2){
                        //
                    }else if(button == 0) {
                        for(int i = 0; i < 3; i++){
                           gause[i]->mouse_release();
                        }
                        for(auto &a :rectangle_){
                            a.mouse_release();
                        }
                    }
                    break;
                case 1://mouse_drag
                    if(button == 0){
                        for(int i = 0; i < 3; i++){
                            gause[i]->mouse_drag(get_viwer_position(x,y));
                        }
                        for(auto &a :rectangle_){
                            a.mouse_drag(get_viwer_position(x,y));
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
private:
    
    bool inner(float x, float y){
        return  ( px < x && x < ( px + width ) && py < y && y < ( py + height ));
    }
    /*
    glm::vec2 get_position(float x, float y){
        float X = x / image_width * width + px;
        float Y = y / image_height * height + py;
        return glm::vec2(X, Y);
    }
    glm::vec2 recover_position(float X, float Y){
        float x = ( X - px ) * image_width / width ;
        float y = ( Y - py ) * image_height / height ;
        return glm::vec2(x, y);
    }
     */
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
};

