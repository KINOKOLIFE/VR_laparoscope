#pragma once
#include "ofApp.h"
//-------------

static Eigen::Matrix4d rotx(double degree)
{
    Eigen::Matrix4d R = Eigen::Matrix4d::Identity(4,4);
    Eigen::Matrix3d AxisAngle;
    Eigen::Vector3d axis;
    axis<<1,0,0;  //x軸を指定
    AxisAngle = Eigen::AngleAxisd( degree / 180.0 * M_PI, axis);
    R.block(0,0,3,3) = AxisAngle;
    return R;
}
static Eigen::Matrix4d roty(double degree)
{
    Eigen::Matrix4d R = Eigen::Matrix4d::Identity(4,4);
    Eigen::Matrix3d AxisAngle;
    Eigen::Vector3d axis;
    axis<<0,1,0;  //y軸を指定
    AxisAngle = Eigen::AngleAxisd( degree / 180.0 * M_PI, axis);
    R.block(0,0,3,3) = AxisAngle;
    return R;
}
static Eigen::Matrix4d rotz(double degree)
{
    Eigen::Matrix4d R = Eigen::Matrix4d::Identity(4,4);
    Eigen::Matrix3d AxisAngle;
    Eigen::Vector3d axis;
    axis<<0,0,1;  //z軸を指定
    AxisAngle = Eigen::AngleAxisd( degree / 180.0 * M_PI, axis);
    R.block(0,0,3,3) = AxisAngle;
    return R;
}

//--------------------
/*open cvが入るまでは使用しない

static Eigen::Matrix4d VecsToMat(cv::Vec3d rvec, cv::Vec3d tvec)
{
    Eigen::Matrix4d H = Eigen::Matrix4d::Identity();
    cv::Mat R;

    // Reallocate from rvec to H
    cv::Rodrigues(rvec, R);

    for (unsigned int m = 0; m < 3; m++)
    {
        for (unsigned int n = 0; n < 3; n++)
        {
            H(m, n) = R.at<double>(m, n);

        }
    }

    // Reallocate from tvec to H
    for (unsigned int m = 0; m < 3; m++)
    {
        H(m, 3) = tvec(m);
    }

    return H;
}
static void MatToVecs(cv::Vec3d &rvec, cv::Vec3d &tvec, Eigen::Matrix4d H)
{
    cv::Vec3d temp1, temp2;
    cv::Mat R = cv::Mat::eye(3, 3, CV_64F);

    
    for (unsigned int m = 0; m < 3; m++)
    {
        for (unsigned int n = 0; n < 3; n++)
        {
            R.at<double>(m, n) = H(m, n);
        }
    }

    
    // Reallocate from H to rvec
    cv::Rodrigues(R, temp1);

    

    // Reallocate from tvec to H
    for (unsigned int m = 0; m < 3; m++)
    {
        temp2(m) = H(m, 3);
    }

    // Return tvec and rvec
    rvec = temp1;
    tvec = temp2;
}
 opencvが入るまでは使用しない*/

static glm::mat4x4 eigen_glm(Eigen::Matrix4d &mat){
    ofMatrix4x4 dest;
    float array[16];
    for(int j=0; j<4 ; j++){
        for(int k=0; k<4; k++){
            array[j*4 + k] = (float)mat(j,k);
        }
    }
    dest.set(array);
    ofMatrix4x4 src = dest.getTransposedOf(dest);
    return glm::mat4x4(src);
};
static Eigen::Matrix4d glm_Eigen(glm::mat4x4  &mat){
    Eigen::Matrix4d dest;
    for(int j=0; j<4 ; j++){
        for(int k=0; k<4; k++){
            dest(j,k) = mat[j][k];
        }
    }
    
    return dest.transpose();
};

//------------------

static void drawTerrios(ofFbo &fbo,ofEasyCam &cam){
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofEnableDepthTest();

    fbo.begin();
        ofClear(0);
    ofBackground(50);
        cam.begin();
            ofDrawGrid(10,10,true,false,false,true);
            ofDrawAxis(10);
        cam.end();
    fbo.end();
};
static void drawTerrios(ofFbo &fbo,ofCamera &cam){
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofEnableDepthTest();

    fbo.begin();
        ofClear(0);
        cam.begin();
            ofDrawGrid(10,10,true,false,false,true);
            ofDrawAxis(10);
        cam.end();
    fbo.end();
};
static void draw_axis(ofFbo &fbo, ofEasyCam &cam, Eigen::Matrix4d &mat){
    fbo.begin();{
        cam.begin();{
            ofPushMatrix();{
                ofMultMatrix(eigen_glm(mat));
                ofDrawAxis(100);
            }ofPopMatrix();
        }cam.end();
    }fbo.end();
};
static void draw_axis(ofFbo &fbo, ofCamera &cam, Eigen::Matrix4d &mat){
    fbo.begin();{
        cam.begin();{
            ofPushMatrix();{
                ofMultMatrix(eigen_glm(mat));
                ofDrawAxis(100);
            }ofPopMatrix();
        }cam.end();
    }fbo.end();
};
static void draw_model(ofFbo &fbo, ofEasyCam &cam, ofxAssimpModelLoader &model, Eigen::Matrix4d &mat, ofColor color){
    fbo.begin();{
        cam.begin();{
            ofPushMatrix();{
                ofMultMatrix(eigen_glm(mat));
                ofPushStyle();{
                    ofEnableAlphaBlending();
                    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                    ofEnableDepthTest();
                    ofSetColor(color);
                    model.drawFaces();
                    ofDrawAxis(100);
                }ofPopStyle();
            }ofPopMatrix();
        }cam.end();
    }fbo.end();
}
static void draw_model(ofFbo &fbo, ofEasyCam &cam, ofxAssimpModelLoader &model, Eigen::Matrix4d &mat){
    fbo.begin();{
        cam.begin();{
            ofPushStyle();{
                ofLight    light;
                ofEnableSeparateSpecularLight();
                ofFloatColor initAmbColor = ofFloatColor(0.8,0.8,0.8,1.0);
                ofFloatColor initDifColor = ofFloatColor(0.7,0.7,0.7);
                ofFloatColor initSpeColor = ofFloatColor(0.6,0.6,0.6);
                light.setAmbientColor(initAmbColor);
                light.setDiffuseColor(initDifColor);
                light.setSpecularColor(initSpeColor);
                light.setPointLight();
                light.enable();{
                    light.setPosition(0,0,30);
                    ofPushMatrix();{
                        ofMultMatrix(glm::inverse(cam.getModelViewMatrix()));
                        //light.setPosition(0,0,-20);
                    }ofPopMatrix();
                    ofPushMatrix();{
                        ofMultMatrix(eigen_glm(mat));
                        ofDrawAxis(100);
                       // ofDrawBox(10);
                        model.drawFaces();
                    }ofPopMatrix();
                }light.disable();
            }ofPopStyle();
        }cam.end();
    }fbo.end();
};
static void draw_marker(Eigen::Matrix4d &mat,float length, ofColor stroke, ofColor fill,ofFbo &fbo,ofEasyCam &cam){
    fbo.begin();
         cam.begin();
            ofEnableDepthTest();
    
             ofPushMatrix();
             {
                 ofMultMatrix(eigen_glm(mat));
                 ofSetColor(fill);
                 ofNoFill();
                 ofDrawRectangle(-length / 2.0, -length / 2.0, length ,length);
                 ofSetColor(255,0,0);
                ofFill();
                 //ofDrawCircle(-length / 2.0, -length / 2.0, 0.0 ,10);
             }
             ofPopMatrix();
            ofDisableDepthTest();
         cam.end();
     fbo.end();
}
static void draw_marker(Eigen::Matrix4d &mat,float length, ofColor stroke, ofColor fill,ofFbo &fbo,ofEasyCam &cam,bool cv_to_gl){
    Eigen::Matrix4d mm = Eigen::Matrix4d::Identity(4,4);
    if(cv_to_gl){
        mm = mat * rotx(180);
    }else{
        mm << mat;
    }
    fbo.begin();
         cam.begin();
            ofEnableDepthTest();
             ofPushMatrix();
             {
                 ofPushStyle();{
                     ofMultMatrix(eigen_glm(mm));
                     ofSetColor(fill);
                     ofFill();
                     //ofSetLineWidth(4);
                     ofDrawRectangle(-length / 2.0, -length / 2.0, length ,length);
                     ofSetColor(stroke);
                     ofNoFill();
                     //ofSetLineWidth(0);
                     ofDrawRectangle(-length / 2.0 , -length / 2.0  , length , length);
                     ofDrawAxis(length /2.0);
                 }ofPopStyle();
             }
             ofPopMatrix();
            ofDisableDepthTest();
         cam.end();
     fbo.end();
}
static void draw_board(Eigen::Matrix4d &mat, int gridnum, float marker_length, ofColor stroke, ofColor fill, ofFbo &fbo,ofEasyCam &cam){
    float board_length = ( gridnum - 1 ) * ( marker_length + 2.0 ) + marker_length / 2.0;
    float board_center = board_length / 2.0;
    fbo.begin();
         cam.begin();
            ofEnableDepthTest();
    
             ofPushMatrix();
             {
                 ofMultMatrix(eigen_glm(mat));
                 ofPushMatrix();
                 {
                     ofTranslate(board_center + marker_length / 2.0 , board_center + marker_length / 2.0);
                     ofDrawAxis(marker_length / 2.0);
                 }
                 ofPopMatrix();
                 ofDrawAxis(marker_length / 2.0);
                 ofSetColor(fill);
                 ofNoFill();
                 ofDrawRectangle(marker_length /2.0 , marker_length /2.0 , board_length ,board_length);
                 ofSetColor(255,0,0);
                 ofFill();
                 //ofDrawCircle(-length / 2.0, -length / 2.0, 0.0 ,10);
             }
             ofPopMatrix();
            ofDisableDepthTest();
         cam.end();
     fbo.end();
}

//---local
/*opencvが入りまで未使用
static void draw_marker_center(ofFbo &fbo, ofEasyCam &cam, std::vector<cv::Vec3d> &points, ofColor c, float rad, bool fiiiinsde){
    fbo.begin();{
        cam.begin();{
            ofPushStyle();{
                ofSetColor(c);
                if(fiiiinsde){
                    ofFill();
                }else{
                    ofNoFill();
                }
                for( auto p : points){
                    ofDrawSphere(p(0), p(1), p(2), rad);
                }
            }ofPopStyle();
        }cam.end();
    }fbo.end();
}
*/
