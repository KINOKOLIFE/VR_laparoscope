#pragma once
#include <iostream>
#include <vector>
#include "ofApp.h"
#include "sheared_stracture.h"
class gbf{
public:
    ofMatrix4x4 projection_matrix;
    ofMesh quad;
    ofShader geo_shader;
    ofShader phong_shader;
    ofFbo g_buffer;
    bool use_gbf;
    bool endoscope = false;
    Eigen::Matrix4d camera_mat;
    gbf(){
        //-------shader
        geo_shader.load("shader/geo_shader");
        phong_shader.load("shader/phong_shader");
        ofDisableArbTex();
        vector<GLint> formats = { GL_RGBA16F, GL_RGBA16F, GL_RGBA16F };
        ofFbo::Settings settings;
        settings.width = 480;
        settings.height = 270;
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
        camera_mat.setIdentity();
        
    }

    void update(ofFbo &gfbo, ofEasyCam &easycam, ofCamera &camera){
        if(endoscope){
            update(gfbo, camera);
            
        }else{
            update(gfbo, easycam);
        }
    }
    void update(ofFbo &gfbo, ofEasyCam &easycam){
        gfbo.begin();{
            ofClear(0);
            glEnable(GL_DEPTH_TEST);
            //1. render geometry to G-Buffer-------------------------------
            ofMatrix4x4 viewMatrix;
            g_buffer.begin();{
                g_buffer.activateAllDrawBuffers();
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                easycam.begin();{
                    projection_matrix = easycam.getProjectionMatrix();
                    viewMatrix = ofGetCurrentViewMatrix();
                    geo_shader.begin();{
                        geo_shader.setUniformMatrix4f("view", viewMatrix);
                        geo_shader.setUniformMatrix4f("projection", projection_matrix);
                        geo_shader.setUniform1i("u_isLight", 0);
              
       
                        geo_shader.setUniform1i("u_isLight", 1);
                        geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5 ,1.0));
                        //realsense_mesh.draw();
                        ofMesh m;
                        geo_shader.setUniform4f("color" , glm::vec4(1.0 ,0.0 ,0.0 ,1.0));
                        m = rectMesh(0,0,100,100,true);
                        setNormals(m);
                        m.draw();
                        ofPushMatrix();
                        ofTranslate(0,0, 250);
                        geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5,1.0));
                     
                     
                        ofPopMatrix();
                        geo_shader.setUniform4f("color" , glm::vec4(0.4 ,0.4 ,0.4 ,0.6));
                        //ofDrawGrid(10,10,false,true,false,false);
                        ofDrawGrid(10,10,false,false,false,true);
                        ofPushMatrix();
                        ofTranslate(0.1,0.1,0.1);
                        geo_shader.setUniform4f("color" , glm::vec4(1.0 ,0.0 ,0.0 ,1.0));
                        ofDrawLine(0,0,0,100,0,0);
                        geo_shader.setUniform4f("color" , glm::vec4(0.0 ,1.0 ,0.0 ,1.0));
                        ofDrawLine(0,0,0,0,100,0);
                        geo_shader.setUniform4f("color" , glm::vec4(0.0 ,0.0 ,1.0 ,1.0));
                        ofDrawLine(0,0,0,0,0,100);
                        ofPopMatrix();
                    }geo_shader.end();
                }easycam.end();
            }g_buffer.end();
            // 3. lighting Pass--------------------------------------------
            glDisable(GL_DEPTH_TEST);
            phong_shader.begin();{
                phong_shader.setUniformTexture("gPosition", g_buffer.getTexture(0), 0);
                phong_shader.setUniformTexture("gNormal", g_buffer.getTexture(1), 1);
                phong_shader.setUniformTexture("gAlbedo", g_buffer.getTexture(2), 2);
                phong_shader.setUniform3f("u_lightPos", ofVec3f(20.0, 50.0, 30.0) * viewMatrix);
                phong_shader.setUniform3f("viewPos", easycam.getPosition());
                quad.draw();
            }phong_shader.end();
        }gfbo.end();
    }
    void update(ofFbo &gfbo, ofCamera &cam){
        gfbo.begin();{
            ofClear(0);
            glEnable(GL_DEPTH_TEST);
            //1. render geometry to G-Buffer-------------------------------
            ofMatrix4x4 viewMatrix;
            g_buffer.begin();{
                g_buffer.activateAllDrawBuffers();
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                cam.begin();{
                    projection_matrix = cam.getProjectionMatrix();
                    viewMatrix = ofGetCurrentViewMatrix();
                    geo_shader.begin();{
                        ofPushMatrix();{
                            glm::mat4x4 mv = eigen_glm(camera_mat);
                            glm::mat4x4 mvi = glm::inverse(mv);
                            ofMultMatrix(mvi);
                            geo_shader.setUniformMatrix4f("view", viewMatrix);
                            geo_shader.setUniformMatrix4f("projection", projection_matrix);
                            geo_shader.setUniform1i("u_isLight", 0);
                  
           
                            geo_shader.setUniform1i("u_isLight", 1);
                            geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5 ,1.0));
                            //realsense_mesh.draw();
                            ofMesh m;
                            geo_shader.setUniform4f("color" , glm::vec4(1.0 ,0.0 ,0.0 ,1.0));
                            m = rectMesh(0,0,100,100,true);
                            setNormals(m);
                            m.draw();
                            ofPushMatrix();
                            ofTranslate(0,0, 250);
                            geo_shader.setUniform4f("color" , glm::vec4(0.5 ,0.5 ,0.5,1.0));
                         
                         
                            ofPopMatrix();
                            geo_shader.setUniform4f("color" , glm::vec4(0.4 ,0.4 ,0.4 ,0.6));
                            //ofDrawGrid(10,10,false,true,false,false);
                            ofDrawGrid(10,10,false,false,false,true);
                            ofPushMatrix();
                            ofTranslate(0.1,0.1,0.1);
                            geo_shader.setUniform4f("color" , glm::vec4(1.0 ,0.0 ,0.0 ,1.0));
                            ofDrawLine(0,0,0,100,0,0);
                            geo_shader.setUniform4f("color" , glm::vec4(0.0 ,1.0 ,0.0 ,1.0));
                            ofDrawLine(0,0,0,0,100,0);
                            geo_shader.setUniform4f("color" , glm::vec4(0.0 ,0.0 ,1.0 ,1.0));
                            ofDrawLine(0,0,0,0,0,100);
                            ofPopMatrix();
                        }ofPopMatrix();
                    }geo_shader.end();
                }cam.end();
            }g_buffer.end();
            // 3. lighting Pass--------------------------------------------
            glDisable(GL_DEPTH_TEST);
            phong_shader.begin();{
                phong_shader.setUniformTexture("gPosition", g_buffer.getTexture(0), 0);
                phong_shader.setUniformTexture("gNormal", g_buffer.getTexture(1), 1);
                phong_shader.setUniformTexture("gAlbedo", g_buffer.getTexture(2), 2);
                phong_shader.setUniform3f("u_lightPos", ofVec3f(20.0, 50.0, 30.0) * viewMatrix);
                phong_shader.setUniform3f("viewPos", cam.getPosition());
                quad.draw();
            }phong_shader.end();
        }gfbo.end();
    }
};
