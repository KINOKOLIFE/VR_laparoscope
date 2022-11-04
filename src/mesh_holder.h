#pragma once
#include "ofApp.h"
#include "sheared.h"

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
};

class meshController{
public:
    int current_page;
    string parent_name;
    float col[3] = { 1.0f,0.0f,0.2f };
    bool is_picker_target;
    bool hide_or_show;
    string given_name;
    
    meshController(){
        
    }
    void transition_page(){
        if(  meshHolder::mesh_imported.size() > 0 ){
            glm::vec4 c = meshHolder::mesh_imported[current_page].color;
            col[0] = c.r; col[0] = c.g; col[0] = c.b;
            is_picker_target = false;
            hide_or_show = meshHolder::mesh_imported[current_page].dysplay;
            parent_name = meshHolder::mesh_imported[current_page].parent_name ;
            given_name = meshHolder::mesh_imported[current_page].given_name;
        }else{
            given_name = "";
            hide_or_show = false;
            is_picker_target = false;
            parent_name = "";
        }
        for( auto  m_i : meshHolder::mesh_imported ){
            m_i.pick_target = false;
        }
    }
    void hide(){
        if(  meshHolder::mesh_imported.size() > 0 ){
            meshHolder::mesh_imported[current_page].dysplay = true;
        }
    }
    void show(){
        if(  meshHolder::mesh_imported.size() > 0 ){
            meshHolder::mesh_imported[current_page].dysplay = false;
        }
    }
    
    void add_mesh(ofMesh &m, string parent_name){
        meshContainer mc;
        mc.mesh = m;
        mc.dysplay = true;
        mc.parent_isDysplay = true;
        mc.parent_name = parent_name;
        mc.matrix = glm::mat4(1.0f);
        mc.given_name = to_string(current_page);
        meshHolder::mesh_imported.push_back(mc);
        transition_page();
    }
    void delet_mesh(){
        if(  meshHolder::mesh_imported.size() > 0 ){
            meshHolder::mesh_imported.erase( meshHolder::mesh_imported.begin() + current_page, meshHolder::mesh_imported.begin() + current_page +1 );
        }
    }
    
    void toggle_picker_target_button(){
        for( auto  m_i : meshHolder::mesh_imported ){
            m_i.pick_target = false;
        }
        if(  meshHolder::mesh_imported.size() > 0 ){
            meshHolder::mesh_imported[current_page].pick_target = is_picker_target;
        }
        
    }
    void forward_page(){
        current_page ++;
        if( current_page > meshHolder::mesh_imported.size() - 1){
            current_page = meshHolder::mesh_imported.size() - 1;
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
        meshHolder::mesh_imported[current_page].color = glm::vec4( col[0],  col[1],  col[2] , 1.0);
    }
    void change_color_all(){
        string parent_name;
        if(meshHolder::mesh_imported.size() > 0){
            parent_name = meshHolder::mesh_imported[current_page].parent_name;
        }
        for( int i = 0; i < meshHolder::mesh_imported.size(); i++){
            if( meshHolder::mesh_imported[i].parent_name == parent_name ){
                meshHolder::mesh_imported[i].color = glm::vec4( col[0],  col[1],  col[2] , 1.0);
            }
        }
    }
};

