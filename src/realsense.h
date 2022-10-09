#pragma once
#include "ofApp.h"
#include <librealsense2/rs.hpp>
#include "EQKF.hpp"

class rs265: public ofThread{
private:
    rs2::pipeline p;
    rs2::config cfg;
    cv::Mat intrinsic, distortion;
    cv::Mat distortion_;
    ofxAssimpModelLoader realsense_model;
    bool first_pass = true;
    Eigen::Matrix4d start_position;
    //ofImage fisheye_left_image;
    EQKF KF;
    int start = std::clock();
public:
    Eigen::Matrix4d t265_rawoutput;
    Eigen::Matrix4d t265_collect;
    cv::Mat fisheye_left_cvimage;
    cv::Mat fisheye_right_cvimage;
    float iProcessCov = 20;
    float iMeasurementCov = 100;
//------------
    void stop(){
        p.stop();
    }
    void setup(){
        t265_rawoutput.setIdentity();
        t265_collect.setIdentity();
        //cv::FileStorage fs("camera.xml", cv::FileStorage::READ);
        //fs["intrinsic"] >> intrinsic;
        //fs["distortion"] >> distortion;
        //std::cout<<intrinsic<<std::endl;
        //std::cout<<distortion<<std::endl;
        //fs.release();
        //distortion_ = (cv::Mat1d(1, 4) << 0,0,0,0,0);
        //realsense_model.loadModel("realsense_model/t265.obj",true);
        //realsense_model.setScaleNormalization(false);
        fisheye_left_cvimage = cv::Mat::zeros(cv::Size(848, 800),CV_8UC1);
        //fisheye_left_image.allocate(848, 800, OF_IMAGE_GRAYSCALE);
    }
    bool connect(){
        std::cout<<"t265 conneting....."<<std::endl;
        cfg.enable_stream(RS2_STREAM_POSE);
        cfg.enable_stream(RS2_STREAM_FISHEYE,1, RS2_FORMAT_Y8);
        cfg.enable_stream(RS2_STREAM_FISHEYE,2, RS2_FORMAT_Y8);
        try{
            auto profile = p.start(cfg);
            const int fisheye_sensor_idx = 1;

            // Get fisheye sensor intrinsics parameters
            rs2::stream_profile fisheye_stream = profile.get_stream(RS2_STREAM_FISHEYE, fisheye_sensor_idx);
            rs2_intrinsics intrinsics = fisheye_stream.as<rs2::video_stream_profile>().get_intrinsics();
            std::cout<<intrinsics.fx<<" "<<intrinsics.fy<<" "<<intrinsics.ppx<<" "<<intrinsics.ppy<<std::endl;
            }
            catch(...)
            {
                std::cout<<"cannot connect t265"<<std::endl;
                return false;
            }
        std::cout<<"t265 connet establish"<<std::endl;
        return true;
    }
    void draw(ofFbo &fbo, ofEasyCam &cam){
        
        //draw_model(fbo, cam, realsense_model, t265_rawoutput, ofColor(200));
        //draw_model(fbo, cam, realsense_model, t265_collect, ofColor(200));
      
    }
//-----------------
    void threadedFunction(){
        while(isThreadRunning()){
            KF.q = static_cast<double>(iProcessCov) / 100.0; ;
            KF.r = static_cast<double>(iMeasurementCov) / 100.0;;
            rs2::frameset frameset;
            try{
                frameset = p.wait_for_frames();
                if (rs2::pose_frame pose_frame = frameset.first_or_default(RS2_STREAM_POSE))
                {
                    rs2_pose pose_sample = pose_frame.get_pose_data();
                    Eigen::Vector3d T;
                    T <<(double)pose_sample.translation.x * 1000.0,
                    (double)pose_sample.translation.y * 1000.0,
                    (double)pose_sample.translation.z * 1000.0;
                    Eigen::Quaterniond q = Eigen::Quaterniond((double)pose_sample.rotation.w, (double)pose_sample.rotation.x, (double)pose_sample.rotation.y, (double)pose_sample.rotation.z);//w,x,y,z
                    Eigen::Matrix3d m(q);
                    Eigen::Matrix4d Mat;
                    Mat.setIdentity();
                    Mat.block<3,1>(0,3) = T;
                    Mat.block<3,3>(0,0) = m;
                  
                    t265_rawoutput = Eigen::Matrix4d(Mat);
                    KF.dt = static_cast<double>(std::clock() - start)/ 1000.0;
                    KF.Predict();
                    t265_collect = KF.Correct(t265_rawoutput);
                    start = std::clock();
                }
                if(rs2::video_frame fisheye_left = frameset.get_fisheye_frame(1)){
                    fisheye_left_cvimage = cv::Mat(cv::Size(848, 800), CV_8UC1, (void*)fisheye_left.get_data());
                }
            }
            catch(...)
            {
                std::cout<<"t265 hung up"<<std::endl;
                this->stopThread();
            }
        }
    }
};
