#pragma once
#include <iostream>
#include <vector>
#include "ofApp.h"
#include "sheared_stracture.h"
//https://qiita.com/y_UM4/items/7647fd9fc19e60ec5822
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
   
    gbuffer(){
        
    }
    void setup(float width, float height){
        fbo.allocate(width, height);
        //-------shader
        geo_shader.load("shader/geo_shader");
        phong_shader.load("shader/phong_shader");
        ofDisableArbTex();
        vector<GLint> formats = { GL_RGBA16F, GL_RGBA16F, GL_RGBA16F };
        ofFbo::Settings settings;
        settings.width = width;
        settings.height = height;
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
        
    }
    void set_camera_param(float fov){
        camera.setFov(fov);
        g_buffer.begin();
        camera.begin();
            projection_matrix = camera.getProjectionMatrix();
        camera.end();
        g_buffer.end();

        
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
                                geo_shader.setUniform1i("u_isLight", 0);
                                geo_shader.setUniform1i("u_isLight", 1);
                                geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5 ,1.0));
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
