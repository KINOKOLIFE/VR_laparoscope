#pragma once
#include "ofApp.h"

class Picker {
public:
    float px, py, width, height;
    int target_mesh_index;
    ofCamera camera;
    ofFbo g_buffer_fbo;
    ofMatrix4x4 projection_matrix;
    float focal_length;
    //------------------
    Picker(){
        
    }
    
    Picker(float _px, float _py, float _width, float _height)
    :px(_px), py(_py), width(_width), height(_height)
    {
        
    }
    void setup(float _px, float _py, float _width, float _height){
        px = _px;   py = _py;   width = _width; height = _height;
        g_buffer_fbo.allocate(width , height);
        ofRegisterMouseEvents(this);
    }
    void setup(){
        g_buffer_fbo.allocate(width , height);
        ofRegisterMouseEvents(this);
    }
    void set_target_mesh(int index){
        if(meshHolder::mesh_imported.size() > 0){
            target_mesh_index = index;
        }
    }
    
    void set_camera_param(float fov){
        camera.setFov(fov);
        g_buffer_fbo.begin();
        camera.begin();
            projection_matrix = camera.getProjectionMatrix();
        camera.end();
        g_buffer_fbo.end();
        focal_length = height / 2.0 / tan(fov / 2.0 * M_PI /180);
    }
    void set_camera_param(ofEasyCam &camera){
        g_buffer_fbo.begin();
        camera.begin();
            projection_matrix = camera.getProjectionMatrix();
        camera.end();
        g_buffer_fbo.end();
        float fov = camera.getFov();
        focal_length = height / 2.0 / tan(fov / 2.0 * M_PI /180);
    }
    void pick(float mx, float my, glm::mat4 &camera_position){
        if(inner(mx, my) && meshHolder::enable_picker ){
            for( auto c_m : meshHolder::mesh_imported){
                if( c_m.pick_target ){
                    //-------------------候補を絞る。裏面、も選択されている。
                    glm::mat4 model = glm::inverse(camera_position);
                    int num_vertex = c_m.mesh.getNumVertices();
                    glm::vec3 mouse(mx - px, my - py, 0);
                    vector<glm::vec3> candities_vertex;
                    vector<int> candities_vertix_index;
                    g_buffer_fbo.begin();{
                            camera.begin();{
                                for(int i = 0; i < num_vertex; i++) {
                                    glm::vec3 v = model * glm::vec4(c_m.mesh.getVertex(i), 1.0);
                                    glm::vec3 cur = camera.worldToScreen(v);
                                    //これは fbo cameraの中でしか動かない
                                    float distance = glm::distance(cur, mouse);
                                    if(distance <  20){
                                        //candities_vertex.push_back(mesh.getVertex(i));
                                        candities_vertex.push_back(v);
                                        candities_vertix_index.push_back(i);
                                    }
                                }
                            }camera.end();
                    }g_buffer_fbo.end();
                    //-------------------レイキャスティング　表面のみ選択
                    glm::vec3 mouse_w =  glm::vec3(mouse.x - width / 2.0, - (mouse.y - height / 2.0), - focal_length);
                    glm::vec3 N = glm::normalize(mouse_w);

                    bool out = false;
                    for(int itr = 0; itr < 500; itr ++){
                        glm::vec3 apex =  N * itr    ;
                        for(int i = 0; i < candities_vertex.size(); i++){
                            float distance3d = glm::distance(apex, candities_vertex[i]);
                            if(distance3d < 20){
                                meshHolder::nearlest_vertex_from_picker = candities_vertix_index[i];
                                out = true;
                                break;
                            }
                        }
                        if(out){
                            break;
                        }
                    }
                }
            }
        }
    }
    void mouseMoved(ofMouseEventArgs & args){
    }
    void mouseDragged(ofMouseEventArgs & args){};
    void mousePressed(ofMouseEventArgs & args){};
    void mouseReleased(ofMouseEventArgs & args){

    };
    void mouseScrolled(ofMouseEventArgs & args){};
    void mouseEntered(ofMouseEventArgs & args){};
    void mouseExited(ofMouseEventArgs & args){};
    bool inner(float mx, float my){
        return px < mx && py < my && mx < px + width && my < py + height;
    }
    
};
