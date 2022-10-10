#pragma once
#include "ofApp.h"
#include <librealsense2/rs.hpp>
#include "EQKF.hpp"

class testthread : public ofThread {
public:
    rs2::pipeline p;
    rs2::config cfg;
    cv::Mat intrinsic, distortion;
    cv::Mat distortion_;
    ofxAssimpModelLoader realsense_model;
    bool first_pass = true;
    Eigen::Matrix4d start_position;
    ofImage fisheye_left_image;
    EQKF KF;
    int start = std::clock();
    Eigen::Matrix4d t265_rawoutput;
    Eigen::Matrix4d t265_collect;
    cv::Mat fisheye_left_cvimage;
    cv::Mat fisheye_right_cvimage;
    cv::Mat colorimg;
    float iProcessCov = 20;
    float iMeasurementCov = 100;
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> parameters;
    bool prepared;
    vector<int> markerIdL_;
    std::vector<cv::Vec3d> tvecs_;
    std::vector<cv::Vec3d> rvecs_;
//------------
    /*
    void stop(){
        p.stop();
    }
     */
    //rs265() = default;
    void setup(){
        t265_rawoutput.setIdentity();
        t265_collect.setIdentity();
        fisheye_left_cvimage = cv::Mat::zeros(cv::Size(848, 800),CV_8UC1);
        colorimg = cv::Mat::zeros(cv::Size(848, 800),CV_8UC1);
        fisheye_left_image.allocate(848, 800, OF_IMAGE_GRAYSCALE);
        cv::FileStorage fs("/Users/takakiyoshiroshi/Downloads/of_v0.11.0_osx_release/apps/myApps/VR_laparoscope/bin/data/camera.xml", cv::FileStorage::READ);
        fs["intrinsic"] >> intrinsic;
        fs["distortion"] >> distortion;
        std::cout<<intrinsic<<std::endl;
        fs.release();
 
        
        realsense_model.loadModel("realsense_model/t265.obj",true);
        realsense_model.setScaleNormalization(false);
        fisheye_left_cvimage = cv::Mat::zeros(cv::Size(848, 800),CV_8UC1);
        fisheye_left_image.allocate(848, 800, OF_IMAGE_GRAYSCALE);
        //distortion_ = (cv::Mat1d(1, 4) << 0,0,0,0,0);
        //--aruco
        dictionary = getPredefinedDictionary(cv::aruco::DICT_4X4_250);
        parameters = cv::aruco::DetectorParameters::create();
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
            //std::cout<<intrinsics.fx<<" "<<intrinsics.fy<<" "<<intrinsics.ppx<<" "<<intrinsics.ppy<<std::endl;
            }
            catch(...)
            {
                std::cout<<"cannot connect t265"<<std::endl;
                return false;
            }
        std::cout<<"t265 connect establish"<<std::endl;
        prepared = true;
        return true;
    }
    void threadedFunction() {
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
                    vector<vector<cv::Point2f>> markerCornersL, rejectedCandidatesL;
                    vector<int> markerIdsL;
                    detectMarkers(fisheye_left_cvimage, dictionary, markerCornersL, markerIdsL, parameters, rejectedCandidatesL);
                    cv::cvtColor(fisheye_left_cvimage, colorimg, cv::COLOR_GRAY2RGB);
                    cv::aruco::drawDetectedMarkers(colorimg, markerCornersL, markerIdsL);
                    std::vector<cv::Vec3d> rvecs, tvecs;
                    cv::aruco::estimatePoseSingleMarkers(markerCornersL, 26, intrinsic, distortion, rvecs, tvecs);
                    for( int i = 0; i < markerIdsL.size(); i++){
                        cv::aruco::drawAxis(colorimg, intrinsic, distortion, rvecs[i], tvecs[i], 13);
                    }
                    markerIdL_ = markerIdsL;
                    lock();{
                        tvecs_ = tvecs;
                        rvecs_ = rvecs;
                    }unlock();
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