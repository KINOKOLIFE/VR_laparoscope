#pragma once
#include "ofApp.h"

struct meshContainer{
    ofMesh mesh;
    glm::mat4 matrix;
    glm::vec4 color;
    string given_name;
    string parent_name;
    bool dysplay;
    bool parent_isDysplay;
    bool pick_target;
    bool useasskin;
    int camera_port_vertex;
    int forceps1_port_vertex;
    int forceps2_port_vertex;
    float specular_material = 0.8;
    float diffuse_material= 0.8;
    float ambient_material= 0.8;
    float emmisive_material= 0.8;

};

class meshHolder{
public:
    static meshContainer mesh_staff[10];
    static vector<meshContainer> mesh_imported;
    static vector<meshContainer> mesh_captured;
    static int picker_target_mesh_id;
    static int nearlest_vertex_id;
    bool is_picker_target;
    static bool enable_picker;
    static int nearlest_vertex_from_picker;

    int current_page;
    string parent_name;
    float col[3] = { 1.0f,0.0f,0.2f };
    bool hide_or_show;
    string given_name;
    
    meshHolder(){
        
    }
    void transition_page(){
        if(  mesh_imported.size() > 0 ){
            glm::vec4 c = mesh_imported[current_page].color;
            col[0] = c.r; col[1] = c.g; col[2] = c.b;
            is_picker_target = false;
            hide_or_show = mesh_imported[current_page].dysplay;
            parent_name = mesh_imported[current_page].parent_name ;
            given_name = mesh_imported[current_page].given_name;
        }else{
            given_name = "";
            hide_or_show = false;
            is_picker_target = false;
            parent_name = "";
        }
        for( auto  m_i : mesh_imported ){
            m_i.pick_target = false;
        }
        Controller::enable_picker = false;
    }
    void hide(){
        if(  mesh_imported.size() > 0 ){
            mesh_imported[current_page].dysplay = false;
        }
        hide_or_show = false;
    }
    void show(){
        if(  mesh_imported.size() > 0 ){
            mesh_imported[current_page].dysplay = true;
        }
        hide_or_show = true;
    }
    
    void add_mesh(ofMesh &m, string parent_name){
        meshContainer mc;
        mc.mesh = m;
        mc.dysplay = true;
        mc.parent_isDysplay = true;
        mc.parent_name = parent_name;
        mc.matrix = glm::mat4(1.0f);
        mc.given_name = to_string(mesh_imported.size());
        mesh_imported.push_back(mc);
        transition_page();
    }
    void delet_mesh(){
        if(  mesh_imported.size() > 0 ){
            cout<<mesh_imported.size()<<endl;
            mesh_imported.erase( mesh_imported.begin() + current_page, mesh_imported.begin() + current_page + 1 );
        }
    }
    bool isabailabe(){
        if(  mesh_imported.size() > 0 ){
            return true;
        }else{
            return false;
        }
    }
    
    void toggle_picker_target_button(){
        for( auto  m_i : mesh_imported ){
            m_i.pick_target = false;
        }
        if(  mesh_imported.size() > 0 ){
            mesh_imported[current_page].pick_target = Controller::enable_picker;
        }
        
    }
    void forward_page(){
        current_page ++;
        if( current_page > mesh_imported.size() - 1){
            current_page = mesh_imported.size() - 1;
        }
        transition_page();
    }
    void backward_page(){
        current_page --;
        if( current_page < 0){
            current_page = 0;
        }
        transition_page();
    }
    void change_color(){
        mesh_imported[current_page].color = glm::vec4( col[0],  col[1],  col[2] , 1.0);
    }
    void change_color_all(){
        string parent_name;
        if(mesh_imported.size() > 0){
            parent_name = mesh_imported[current_page].parent_name;
        }
        for( int i = 0; i < mesh_imported.size(); i++){
            if( mesh_imported[i].parent_name == parent_name ){
                mesh_imported[i].color = glm::vec4( col[0],  col[1],  col[2] , 1.0);
            }
        }
    }
};

