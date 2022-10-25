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
    virtual void draw(ofFbo &fbo){
        fbo.begin();
            ofSetColor(ofColor::fromHsb(hue, 255, 255));
        if(focus){
            ofSetColor(ofColor::fromHsb(hue, 255, 100));
        }
        
            ofDrawCircle(px, py, radius);
        if(selected){
            ofSetColor(ofColor::fromHsb(255-hue, 255, 255));
            ofNoFill();
            ofDrawCircle(px, py, radius + 1);
            ofFill();
        }
       
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




class Viewer{
public:
    bool hover;
    float px, py;
    float width, height;
    //float image_width, image_height; //もしかしたら不要かもしれない
    std::vector<object_> rectangle_;
    std::vector<std::shared_ptr<object_> > gauge;
    std::vector<object_> pebbles;
    float pixelper10mm;
    float croppedHight;
    float croppedWIdth;
    float croppedPx;
    float croppedPy;
    float _hue = 60;
    float _radius = 4;
    bool _fill = false;
    bool _erase = false;
    
    imageStack imgstk;
    
    Viewer(float _x, float _y, float _w, float _h):px(_x), py(_y), width(_w), height(_h){

        //https://qiita.com/MusicScience37/items/1ba81db1592c974f0632
        auto d1 = std::make_shared<Holizontal>();
        auto d2 = std::make_shared<Holizontal>();
        auto h1 = std::make_shared<Vertical>();
        gauge.push_back(d1);
        gauge.push_back(d2);
        gauge.push_back(h1);
        gauge[0]->set_param(width - 20, 20, 40,  6);
        gauge[1]->set_param(width - 20, 60, 40,  6);
        gauge[2]->set_param(width / 2.0,10, 100, 6);
        rectangle_.push_back(object_(10,10,                  60,6));
        rectangle_.push_back(object_(width - 20, 10,         60,6));
        rectangle_.push_back(object_(width - 20,height - 10, 60,6));
        rectangle_.push_back(object_(10,height - 10,         60,6));
        try{
            ofJson js;
            ofFile file("us_viewer_setting.json"); //aruco marker arrayの頂点ファイル
            file >> js;
            gauge[0]->py = js["pt"][0][0];
            gauge[1]->py = js["pt"][0][1];
            gauge[2]->px = js["pt"][0][2];
            rectangle_[0].px = js["pt"][0][3];
            rectangle_[0].py = js["pt"][0][4];
            rectangle_[2].px = js["pt"][0][5];
            rectangle_[2].py = js["pt"][0][6];
            rectangle_[1].px = rectangle_[2].px;
            rectangle_[1].py = rectangle_[0].py;
            rectangle_[3].px = rectangle_[0].px;
            rectangle_[3].py = rectangle_[2].py;
            std::cout<<"read us setting file"<<std::endl;
            }
            catch(...)
            {
                std::cout<<"cannot read file"<<std::endl;
            }
    }
    Viewer(){
    }
    void fbo_allocate(ofFbo &fbo){
        fbo.allocate(width, height);
    }
    void setImageSize(float w, float h){
        
    }
    void changeColor(){
        for(auto &a :pebbles){
            if(a.selected) {
                a.hue = _hue * 256.0;
            };
        }
    }
    void changeRadius(){
        for(auto &a :pebbles){
            if(a.selected) {
                a.radius = _radius;
            };
        }
    }
    void delete_pebble(){
        for(int i = 0; i < pebbles.size(); i++){
            if(pebbles[i].selected){
                pebbles.erase(pebbles.begin() + i, pebbles.begin() + i + 1);
            }
        }
    }
    void togglec_fill(){
        if(_fill){
            _erase = false;
        }
    }
    void togglec_erase(){
        if(_erase){
            _fill = false;
        }
    }
    void setStack(imageStack &stk){
        rectangle_[0].px = stk.px;
        rectangle_[0].py = stk.py;
        rectangle_[1].px = stk.px + stk.w;
        rectangle_[1].py = stk.py;
        rectangle_[2].px = stk.px + stk.w;
        rectangle_[2].py = stk.py + stk.h;
        rectangle_[3].px = stk.px;
        rectangle_[3].py = stk.py + stk.h;
        gauge[0]->py = stk.gauge0;
        gauge[1]->py = stk.gauge1;
        gauge[2]->px = stk.gauge2;
        
    }
    void setStack(vector<imageStack> &stks, int p){
        
        rectangle_[0].px = stks[p].px;
        rectangle_[0].py = stks[p].py;
        rectangle_[1].px = stks[p].px + stks[p].w;
        rectangle_[1].py = stks[p].py;
        rectangle_[2].px = stks[p].px + stks[p].w;
        rectangle_[2].py = stks[p].py + stks[p].h;
        rectangle_[3].px = stks[p].px;
        rectangle_[3].py = stks[p].py + stks[p].h;
        gauge[0]->py = stks[p].gauge0;
        gauge[1]->py = stks[p].gauge1;
        gauge[2]->px = stks[p].gauge2;
        
    }
    void getStack(vector<imageStack> &stks, int p){
        stks[p].px = rectangle_[0].px;
        stks[p].py = rectangle_[0].py;
        stks[p].w =  rectangle_[1].px - rectangle_[0].px;
        stks[p].h =  rectangle_[2].py - rectangle_[0].py;
        
        stks[p].gauge0 = gauge[0]->py;
        stks[p].gauge1 = gauge[1]->py;
        stks[p].gauge2 = gauge[2]->px;
    
        
    }
    void arbitrate(){
        
    }
    void getMarkers(){
        
    }
    void update(ofFbo &fbo){
        fbo.begin();{
            ofClear(0);
        }fbo.end();
        for(int i = 0; i < 3; i++){
            gauge[i]->draw(fbo);
        }
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
        fbo.begin();{
            ofSetColor(ofColor::fromHsb(200, 255, 255));
            ofDrawLine(rectangle_[0].px, rectangle_[0].py,  rectangle_[1].px, rectangle_[1].py);
            ofDrawLine(rectangle_[1].px, rectangle_[1].py,  rectangle_[2].px, rectangle_[2].py);
            ofDrawLine(rectangle_[2].px, rectangle_[2].py,  rectangle_[3].px, rectangle_[3].py);
            ofDrawLine(rectangle_[3].px, rectangle_[3].py,  rectangle_[3].px, rectangle_[1].py);
        }fbo.end();
        for(auto &a :rectangle_){
            a.draw(fbo);
            a.updated = false;
        }
        for(auto &o :pebbles){
            o.draw(fbo);
            o.updated = false;
        }
        //-----calc cropped
        pixelper10mm = abs(gauge[0]->py - gauge[1]->py);
        imgstk.px = rectangle_[0].px;
        imgstk.py = rectangle_[0].py;
        imgstk.w = rectangle_[1].px - rectangle_[0].px;
        imgstk.h = rectangle_[3].py - rectangle_[0].py;
        imgstk.mm_pixel = pixelper10mm;
        imgstk.gauge0 =  gauge[0]->py;
        imgstk.gauge1 =  gauge[1]->py;
        imgstk.gauge2 =  gauge[2]->px;
        //imgstk.gauge
        for(auto p:pebbles){
            if(p.selected){
                _hue = p.hue /256.0;
                _radius = p.radius;
            }
        }

    }
    void reset_gause(){
        gauge[0]->set_param(width - 20, 20, 40,  6);
        gauge[1]->set_param(width - 20, 60, 40,  6);
        gauge[2]->set_param(width / 2.0,10, 100, 6);
        rectangle_[0].set_param(10,10,                  60,6);
        rectangle_[1].set_param(width - 20, 10,         60,6);
        rectangle_[2].set_param(width - 20,height - 10, 60,6);
        rectangle_[3].set_param(10,height - 10,         60,6);
    }
    
    void exit(){
        ofJson j;
        ofJson pt;
        pt[0] = gauge[0]->py;
        pt[1] = gauge[1]->py;
        pt[2] = gauge[2]->px;
        pt[3] = rectangle_[0].px;
        pt[4] = rectangle_[0].py;
        pt[5] = rectangle_[2].px;
        pt[6] = rectangle_[2].py;
        j["pt"].push_back(pt);
        ofSaveJson("us_viewer_setting.json", j);
    }
    void mouse(int x, int y, int button, int function){
        if(inner( x, y )){
            switch (function) {
                case 0://mouseMove
                    for(int i = 0; i < 3; i++){
                        gauge[i]->focus = false;
                        gauge[i]->mouse_move(get_viwer_position(x,y));
                    }
                    for(auto &a :rectangle_){
                        a.focus = false;
                        a.mouse_move(get_viwer_position(x,y));
                    }
                    for(auto &p :pebbles){
                        p.focus = false;
                        p.mouse_move(get_viwer_position(x,y));
                    }
                    break;
                case 2://mouse_press
                    if(button == 0){
                        for(int i = 0; i < 3; i++){
                            gauge[i]->selected = false;
                            gauge[i]->mouse_press(get_viwer_position(x,y));
                        }
                        for(auto &a :rectangle_){
                            a.selected = false;
                            a.mouse_press(get_viwer_position(x,y));
                        }
                        for(auto &p :pebbles){
                            p.selected = false;
                            p.mouse_press(get_viwer_position(x,y));
                        }
                    }
                    break;
                case 3://mouse_releas
                    if(button == 2){
                        for(int i = 0; i < 3; i++){
                            gauge[i]->selected = false;
                        }
                        for(auto &a :rectangle_){
                            a.selected = false;
                        }
                        for(auto &p :pebbles){
                            p.selected = false;
                        }
                        glm::vec2 v = get_viwer_position(x,y);
                        pebbles.push_back(object_(v.x, v.y, _hue * 256.0, _radius));
                        //
                    }else if(button == 0) {
                        for(int i = 0; i < 3; i++){
                            gauge[i]->mouse_release();
                        }
                        for(auto &a :rectangle_){
                            a.mouse_release();
                        }
                        for(auto &p :pebbles){
                            p.mouse_release();
                        }
                    }
                    break;
                case 1://mouse_drag
                    if(button == 0){
                        for(int i = 0; i < 3; i++){
                            gauge[i]->mouse_drag(get_viwer_position(x,y));
                        }
                        for(auto &a :rectangle_){
                            a.mouse_drag(get_viwer_position(x,y));
                        }
                        for(auto &p :pebbles){
                            p.mouse_drag(get_viwer_position(x,y));
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    /*
    glm::vec2 get_onImagePosition(float x, float y, float image_width, float image_height){
        float X = x / width * image_width ;
        float Y = y / height * image_height ;
        return glm::vec2(X, Y);
    }
     */

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

