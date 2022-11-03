#pragma once
#include <iostream>
#include <vector>
#include "ofApp.h"
#include "sheared_stracture.h"
//https://qiita.com/y_UM4/items/7647fd9fc19e60ec5822
struct Container{
    ofMesh mesh;
    glm::mat4 matrix;
    glm::vec4 color;
    string name;
    string parent_name;
    bool dysplay;
    bool parent_isDysplay;
    bool pick_target;
    bool useasskin;
    int camera_port_vertex;
    int forceps1_port_vertex;
    int forceps2_port_vertex;
};

class ContainerManager{
public:
    bool enable_picker;
    int current_page;
    glm::vec4 color;
    string name;
    vector<Container> containers;
    //---color
    float hue;
    float col1[3] = { 1.0f,0.0f,0.2f };
    //---
    
    //-constructor
    ContainerManager(){
        
    }
    void add_mesh(ofMesh &mesh_, string parent_){
        glm::mat4 E(1.0f);
        glm::vec4 color_= glm::vec4( 0.8, 0.8, 0.8 , 1.0);
        this->containers.push_back(Container{mesh_, E, color_, "", parent_, true, true});
    }
    bool available(){
        
        if(containers.size()>0){
            return true;
        }else{
            return false;
        }
       
    }
    string getParent(){
        if(containers.size() > 0){
            return containers[current_page].parent_name;
        }else{
            return string("");
        }
    }
    void forward_page(){
        cout<<"forward"<<endl;
        current_page ++;
        if( current_page > containers.size() - 1){
            current_page = containers.size() - 1;
        }
        col1[0] = containers[current_page].color.r;
        col1[1] = containers[current_page].color.g;
        col1[2] = containers[current_page].color.b;
        
    }
    void backward_page(){
        cout<<"backward"<<endl;
        current_page --;
        if( current_page < 0){
            current_page = 0;
        }
       col1[0] = containers[current_page].color.r;
        col1[1] = containers[current_page].color.g;
        col1[2] = containers[current_page].color.b;
        
    }
    void flip_show(){
        if(containers.size() > 0){
            containers[current_page].dysplay = !containers[current_page].dysplay;
        }
    }
    bool show(){
        if(containers.size()>0){
            if(containers[current_page].dysplay){
            return true;
            }else{
                return false;
            }
        }else
            return false;
    }
    void change_color(){
        ofColor c = ofColor::fromHsb( hue * 255, 255, 255 );
        containers[current_page].color = glm::vec4( col1[0],  col1[1],  col1[2] , 1.0);
    }
    void change_color_group(){
        ofColor c = ofColor::fromHsb( hue * 255, 255, 255 );
        //glm::vec4 g = ImColor::HSV(hue , 1.0f, 1.0f);

        string parent_name;
        if(containers.size() > 0){
            parent_name = containers[current_page].parent_name;
        }
        for( int i = 0; i < containers.size(); i++){
            if( containers[i].parent_name == parent_name ){
                containers[i].color = glm::vec4( c.r/256.0,  c.g/256.0,  c.b/256.0 , 1.0);
            }
        }
    }
    void enable_pick(){
        if(containers.size() > 0){
            if(containers[current_page].pick_target){
                containers[current_page].pick_target = false;
                return;
            }
        }
        for( auto c : containers){
            c.pick_target = false;
        }
        
        if(containers.size() > 0){
            containers[current_page].pick_target = true;
        }
        
    }
    void set_camera_port_vertx_index(int i){
        if(containers.size() > 0 && enable_picker){
            containers[current_page].camera_port_vertex = i;
        }
    }
    void set_forceps1_port_vertx_index(int i){
        if(containers.size() > 0 && enable_picker){
            containers[current_page].forceps1_port_vertex = i;
        }
    }
    void set_forceps2_port_vertx_index(int i){
        if(containers.size() > 0 && enable_picker){
            containers[current_page].forceps2_port_vertex = i;
        }
    }
    
};

class gbuffer{
public:
    ofMatrix4x4 projection_matrix;
    ofMesh quad;
    ofShader geo_shader;
    ofShader phong_shader;
    ofFbo g_buffer;
    ofFbo fbo;
    //----
    ofMatrix4x4 viewMatrix;
    //---
    ofCamera camera;
    //---
    ContainerManager container_manager;

    float px,py;
    float width,height;
    //---
    int NearestVertex_index;
    int port_id;
    //-----------
    gbuffer(){
        
    }

    //--------------------
    void setup(float _px, float _py, float _width, float _height){
        px = _px;
        py = _py;
        width = _width;
        height = _height;
        fbo.allocate(width, height);
        //-------shader
        geo_shader.load("shader/geo_shader");
        phong_shader.load("shader/phong_shader");
        ofDisableArbTex();
        vector<GLint> formats = { GL_RGBA16F, GL_RGBA16F, GL_RGBA16F };
        ofFbo::Settings settings;
        settings.width = _width;
        settings.height = _height;
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
        //----
        quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        quad.addVertex(ofVec3f(1.0, 1.0, 0.0)); // top-right
        quad.addTexCoord(ofVec2f(1.0f, 0.0f));
        quad.addVertex(ofVec3f(1.0, -1.0, 0.0)); //bottom-right
        quad.addTexCoord(ofVec2f(1.0f, 1.0f));
        quad.addVertex(ofVec3f(-1.0, -1.0, 0.0)); //bottom-left
        quad.addTexCoord(ofVec2f(0.0f, 1.0f));
        quad.addVertex(ofVec3f(-1.0, 1.0, 0.0)); //top-left
        quad.addTexCoord(ofVec2f(0.0f, 0.0f));
        //---
        ofRegisterMouseEvents(this); 
        
    }
    void set_camera_param(float fov){
        camera.setFov(fov);
        g_buffer.begin();
        camera.begin();
            projection_matrix = camera.getProjectionMatrix();
        camera.end();
        g_buffer.end();
        
        
    }
    void mouseMoved(ofMouseEventArgs & args){
    }
    void mouseDragged(ofMouseEventArgs & args){};
    void mousePressed(ofMouseEventArgs & args){};
    void mouseReleased(ofMouseEventArgs & args){
        if( inner(args.x, args.y ) && args.button == 2){
            if(port_id == 0){
                container_manager.set_camera_port_vertx_index(NearestVertex_index);
            }
            if(port_id == 1){
                container_manager.set_forceps1_port_vertx_index(NearestVertex_index);
            }
            if(port_id == 2){
                container_manager.set_forceps2_port_vertx_index(NearestVertex_index);
            }
        }
    };
    void mouseScrolled(ofMouseEventArgs & args){};
    void mouseEntered(ofMouseEventArgs & args){};
    void mouseExited(ofMouseEventArgs & args){};
    bool inner(float mx, float my){
        return px < mx && py < my && mx < px + width && my < py + height;
    }
    void pick(float mx, float my, glm::mat4 &camera_position){
        if(inner(mx, my)){
            for( auto c : container_manager.containers){
                if( c.pick_target){
                    glm::mat4 model = glm::inverse(camera_position);
                    int num_vertex = c.mesh.getNumVertices();
                    glm::vec3 mouse(mx - px, my - py, 0);
                    vector<glm::vec3> candities_vertex;
                    vector<int> candities_vertix_index;
                    float fov = camera.getFov();
                    float focal_length = height / 2.0 / tan(fov / 2.0 * M_PI /180); //400 means height
                    fbo.begin();{
                            camera.begin();{
                                for(int i = 0; i < num_vertex; i++) {
                                    glm::vec3 v = model * glm::vec4(c.mesh.getVertex(i), 1.0);
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
                    }fbo.end();
                    //-------------------レイキャスティング
                    glm::vec3 mouse_w =  glm::vec3(mouse.x - width / 2.0, - (mouse.y - height / 2.0), - focal_length);
                    glm::vec3 N = glm::normalize(mouse_w);

                    bool out = false;
                    for(int itr = 0; itr < 500; itr ++){
                        glm::vec3 apex =  N * itr    ;
                        for(int i = 0; i < candities_vertex.size(); i++){
                            float distance3d = glm::distance(apex, candities_vertex[i]);
                            if(distance3d < 20){
                                NearestVertex_index = candities_vertix_index[i];
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
    
    void update(glm::mat4 &camera_position, vector<MeshConatiner> &mesh_conatinar){
        glm::mat4x4 zero_position(1.0f);//camera view matrix
        
        glm::mat4x4 model_view_matrix = glm::inverse(camera_position);
        fbo.begin();{
            ofClear(0);
            glEnable(GL_DEPTH_TEST);
            //1. render geometry to G-Buffer-------------------------------
            g_buffer.begin();{
                g_buffer.activateAllDrawBuffers();
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                camera.begin();{
                    geo_shader.begin();{
                        geo_shader.setUniformMatrix4f("view", zero_position);
                        geo_shader.setUniformMatrix4f("projection", projection_matrix);
                            //---
                        ofPushMatrix();{
                            ofMultMatrix(model_view_matrix);
                            ofFill();
                                geo_shader.setUniform1i("u_isLight", 0);
                                geo_shader.setUniform1i("u_isLight", 1);
                                geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5 ,1.0));
                            
                                //pick
                            for( auto c : container_manager.containers){
                                if( c.pick_target){
                                    ofDrawSphere(c.mesh.getVertex(NearestVertex_index),6);
                                }
                                ofDrawSphere(c.mesh.getVertex(c.camera_port_vertex),6);
                                ofDrawSphere(c.mesh.getVertex(c.forceps1_port_vertex),6);
                                ofDrawSphere(c.mesh.getVertex(c.forceps2_port_vertex),6);
                            }
                         
                                //----MESH_CONATINAIR
                                
                                for(auto &mc : mesh_conatinar){
                                    geo_shader.setUniform1i("u_isLight", 0);
                                    if(mc.use_tecxture){
                                            geo_shader.setUniformTexture("image", mc.fbo.getTexture(), 0);
                                    }else{
                                        geo_shader.setUniform1i("u_isLight", 1);
                                        geo_shader.setUniform4f("color" , mc.color);
                                    }
                                    ofPushMatrix();{
                                        ofMultMatrix(mc.matrix);
                                        mc.mesh.draw();
                                    }ofPopMatrix();
                                }
                                //----CONATINAIR
                           
                                for(auto &c : this->container_manager.containers){
                                    geo_shader.setUniform1i("u_isLight", 1);
                                    geo_shader.setUniform4f("color" , c.color);
                                    if(c.parent_isDysplay && c.dysplay){
                                        ofPushMatrix();{
                                            ofMultMatrix(c.matrix);
                                            c.mesh.draw();
                                        }ofPopMatrix();
                                    }
                                }
                            
                            //--- GRID
                                //--- GRID
                                geo_shader.setUniform1i("u_isLight", 1);
                                geo_shader.setUniform4f("color" , glm::vec4(0.4 ,0.4 ,0.4 ,0.6));
                            
                                ofDrawGrid(10,10,false,false,false,true);
                                //---AXIS
                                ofPushMatrix();{
                                    ofTranslate(0.1,0.1,0.1);
                                    geo_shader.setUniform4f("color" , glm::vec4(1.0 ,0.0 ,0.0 ,1.0));
                                    ofDrawLine(0,0,0,100,0,0);
                                    geo_shader.setUniform4f("color" , glm::vec4(0.0 ,1.0 ,0.0 ,1.0));
                                    ofDrawLine(0,0,0,0,100,0);
                                    geo_shader.setUniform4f("color" , glm::vec4(0.0 ,0.0 ,1.0 ,1.0));
                                    ofDrawLine(0,0,0,0,0,100);
                                }ofPopMatrix();
                        }ofPopMatrix();
                    }geo_shader.end();
                }camera.end();
            }g_buffer.end();
            // 3. lighting Pass--------------------------------------------
            glDisable(GL_DEPTH_TEST);
            phong_shader.begin();{
                phong_shader.setUniformTexture("gPosition", g_buffer.getTexture(0), 0);
                phong_shader.setUniformTexture("gNormal", g_buffer.getTexture(1), 1);
                phong_shader.setUniformTexture("gAlbedo", g_buffer.getTexture(2), 2);
                phong_shader.setUniform3f("u_lightPos", ofVec3f(20.0, 50.0, 30.0) * zero_position);
                phong_shader.setUniform3f("viewPos", glm::vec3(0,0,0));
                quad.draw();
            }phong_shader.end();
        }fbo.end();
    }
    
};
//--endoscope
//float camHeight = 540;
//float camera_matirx_height = 427;
//fov = 2.0 * atan( camHeight / 2.0 / camera_matirx_height ) / M_PI * 180;
// screen height /2.0   :   fy    -> most important code
//cam.setFov(fov);
