#pragma once
#include <iostream>
#include <vector>
#include "ofApp.h"
//#include "sheared.h"


class object_{
    public:
    float px,py;
    float hue;
    bool selected;
    float radius;
    bool focus;
    bool onDrag;
    bool updated;
    float start_px, start_py;
    float strat_mouse_x, start_mouse_y;
    int type;
 
    object_() = default;//https://www.delftstack.com/ja/howto/cpp/array-of-objects-in-cpp/
    object_(float _x, float _y, float _hue, float _radius):px(_x), py(_y), hue(_hue), radius(_radius){
        type = 0;
        selected = true;
    }
    object_(float _x, float _y, float _hue, float _radius, int _type):px(_x), py(_y), hue(_hue), radius(_radius), type(_type){
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
            ofSetColor(ofColor::fromHsb(hue * 255, 255, 255));
        if(focus){
            ofSetColor(ofColor::fromHsb(hue * 255, 255, 100));
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
        ofSetColor(ofColor::fromHsb(120, 255, 255));//Hue,Saturation,Brightness
        ofDrawCircle(px, py, radius);
    }
    virtual void draw_vertical_line(int height){
        py = 10;
        ofSetColor(100);
        ofDrawLine(px, 0, px, height);
        ofSetColor(ofColor::fromHsb(120, 255, 255));//Hue,Saturation,Brightness
        ofDrawCircle(px, py, radius);
    }
    virtual void draw(int width, int height){
    }
    void set_param(float _x, float _y, float _hue, float _radius){
        px = _x; py = _y; hue = _hue; radius = _radius;
    }
};
//-----------------------------------------------------------
class captureStack{
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
    glm::mat4 offset;
    glm::vec4 color;
    
    //-------
    captureStack(int _width, int _height):width(_width), height(_height){
        rectangle_.push_back(object_());
        rectangle_.push_back(object_());
        rectangle_.push_back(object_());
        rectangle_.push_back(object_());
        gauge.push_back(object_(width - 20, 20, 40.0/255.0,  6));
        gauge.push_back(object_(width - 20, 60, 40.0/255.0,  6));
        gauge.push_back(object_(width / 2.0,10, 100.0/255.0, 6));
        offset = glm::mat4(1.0f);
        reset_gauge();
    }
    captureStack(){
        
    }
//---------------mouse call back section-------------------------------------
    void mouseMoved(int x, int y, int button){
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
    }
    void mouseDraged(int x, int y, int button){
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
    }
    void mousePressed(int x, int y, int button){
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
    }
    void mouseReleased(int x, int y, int button){
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
            marker.push_back(object_(x, y, hue, radius));
          
        }else if(button == 0) {
            for(auto &g :gauge){
                g.mouse_release();
            }
            for(auto &a :rectangle_){
                a.mouse_release();
            }
            for(auto &a :marker){
                if(a.selected){
                    radius = a.radius;
                    hue = a.hue;
                }
            }
            for(auto &p :marker){
                p.mouse_release();
            }
        }
    }
//----------------------------------------------------------------------
   
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
    void change_radius(float r){
        for(auto &a :marker){
            if(a.selected){
                a.radius = r;
            }
        }
        radius = r;
    }
    void change_color(float h){
        for(auto &a :marker){
            if(a.selected){
                a.hue = h ;
            }
        }
        hue = h;
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
        gauge[0].set_param(width - 20, 20, 40.0/127.0,  6);
        gauge[1].set_param(width - 20, 60, 40.0/127.0,  6);
        gauge[2].set_param(width / 2.0,10, 100.0/127.0, 6);
        rectangle_[0].set_param(10,10,                  60/255.0,   6);
        rectangle_[1].set_param(width - 20, 10,         60/255.0,   6);
        rectangle_[2].set_param(width - 20,height - 10, 60/255.0,   6);
        rectangle_[3].set_param(10,height - 10,         60/255.0,   6);
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
    void crop(ofImage &img, bool make_transparent){
        float crop_width_on_viewer = rectangle_[1].px - rectangle_[0].px;
        float crop_height_on_viewer = rectangle_[3].py - rectangle_[0].py;
        float mm_pixel = 10.0 / abs(gauge[1].py -  gauge[0].py );  // 10mm =  mm_pixel
        float crop_width = crop_width_on_viewer / width *  img.getWidth();
        float crop_hight = crop_height_on_viewer / height *  img.getHeight();
        float crop_px = rectangle_[0].px / width *  img.getWidth();
        float crop_py = rectangle_[0].py / height *  img.getHeight();
        //float mesh_real_space_width = crop_width * mm_pixel;
        //float mesh_real_space_height = crop_hight * mm_pixel;
        float mesh_real_space_width = crop_width_on_viewer * mm_pixel;
        float mesh_real_space_height = crop_height_on_viewer * mm_pixel;
        this->mesh = rectMesh(0,0,mesh_real_space_width,mesh_real_space_height,true);
        setNormals(this->mesh);
        
        float zero_y = min(gauge[0].py ,  gauge[1].py);
        float mesh_real_space_px = ( rectangle_[0].px - gauge[2].px ) * mm_pixel;
        float mesh_real_space_py = ( rectangle_[0].py - zero_y ) * mm_pixel;
        //cout<< mesh_real_space_width <<" "<<  crop_width_on_viewer * mm_pixel <<endl;
        
        glm::mat4 T(1.0f);
        T[3][0] = mesh_real_space_px;
        T[3][1] = mesh_real_space_py;
        this->offset = T;
        
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
        
        //---transparent
        if(make_transparent){
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
        }
    }
};

//-------------------------------------------------//
//スタックの削除　追加　ページングを行う。


class stack_manager{
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
    int snapCounter; //画像保名前作成用のカウンター
    
    //----
    ofJson js;
    
public:
    int current_page;
    //vector<stack> stacks;
    static vector<captureStack> stacks;
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
 
    stack_manager(){
        
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
//---------mouse call back section---------------------------------
    void mouseMoved(ofMouseEventArgs & args){
        if(inner(args.x, args.y)){
            glm::vec2 vec;
            vec = get_viwer_position(args.x, args.y);
            stacks[current_page].mouseMoved(vec.x, vec.y, 0);
        }
    }
    void mouseDragged(ofMouseEventArgs & args){
        if(inner(args.x, args.y)){
            glm::vec2 vec;
            vec = get_viwer_position(args.x, args.y);
            stacks[current_page].mouseDraged(vec.x, vec.y, args.button);
        }
    };
    void mousePressed(ofMouseEventArgs & args){
        if(inner(args.x, args.y)){
            glm::vec2 vec;
            vec = get_viwer_position(args.x, args.y);
            stacks[current_page].mousePressed(vec.x, vec.y, args.button);
        }
    };
    void mouseReleased(ofMouseEventArgs & args){
        if(inner(args.x, args.y)){
            glm::vec2 vec;
            vec = get_viwer_position(args.x, args.y);
            stacks[current_page].mouseReleased(vec.x, vec.y, args.button);
        }
    };
    void mouseScrolled(ofMouseEventArgs & args){};
    void mouseEntered(ofMouseEventArgs & args){};
    void mouseExited(ofMouseEventArgs & args){};
//--------------------------------------------------------------------------
    
    
    
    void capture(ofImage &image){
        string fileName = "snapshot/snapshot_"+ofToString(snapCounter, 5, '0')+".png";
        if(image.save(fileName)){
            
            stacks.push_back( stacks[ stacks.size() - 1 ] );
 
            stacks[ stacks.size() - 2 ].fileName = fileName;
            //stacks[ stacks.size() - 2 ].mat = m;
  
            stacks[ stacks.size() - 2 ].threshold = this->threshold;
            stacks[ stacks.size() - 2 ].crop(image, true);
            snapCounter ++;
            current_page = stacks.size() - 1;
           
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
        captureStack s(width, height);
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
                current_page = 0;//-------------本当は1が正しいかも知れない！
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
       
    }
    void backward_page(){
        current_page --;
        if( current_page < 0){
            current_page = 0;
        }
        change_page();
    }

    void update(ofImage &uvc_image, glm::mat4 m){
        hue = stacks[current_page].hue;
        radius = stacks[current_page].radius;
        if(current_page != stacks.size() -1){
            fbo.begin();
            ofClear(0,0);
            captured.draw(0,0, width, height);
            fbo.end();
            stacks[current_page].update(fbo);
            if( stacks[current_page].crop_flag ){
                stacks[current_page].crop(captured, true);
            }
        }else{
            fbo.begin();
            ofClear(0,0);
            uvc_image.draw(0,0, width, height);
            fbo.end();
            stacks[current_page].update(fbo);
            stacks[current_page].crop(uvc_image, false);
        }
        if(stacks.size() > 0){
            stacks[stacks.size() -1].mat = m;//^^^^^^必要かも！！
        }
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
            stacks[current_page].crop(captured, true);
        }
    }
    void update_threshold(){
        if(current_page != stacks.size() -1){
            stacks[current_page].threshold = this->threshold;
            stacks[current_page].crop(captured, true);
        }
    }
};
//};

/*
 stack_manager.changeColor .changeRadius
 →stack.changeColorに伝達：
 ・その際にselectされているobjectにも伝達
 ・stackのstack.hue stack.radiusにも伝達
 
 stackでは、右クリックされた際にstack.hue stack.radiusを更新。
 更新をstack_managerに通知できないので、
 manger.update内で、stack_manager.radius, stack_manager.hueを更新
 imGUIは、&stack_manager.radius, &stack_manager.hueとして参照する。
 */
/*
 stackは最も新しいもの stacks[stacks.size()-1 ] は、live画像のクリッピングとして用いている。
 captureすると、現在のstackつまり stacks[stacks.size()-1 ]をpush_backする。
 push_backすると、最も最新の　capture　はstackは stacks[stacks.size() -2 ]となるので、これら対してimageやらmatrixやらを付加する。
 
 */
