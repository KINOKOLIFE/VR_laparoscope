#pragma once
#include "ofApp.h"

#include "ceres/ceres.h"

#include "ceres/rotation.h"
#include <iostream>
#include <vector>
#include <algorithm>    // std::copy
#include <iterator>
#include <map>
using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;


struct fishEyeFit{
    fishEyeFit(double observed_x, double observed_y, double* param)
    :x(observed_x), y(observed_y), fx(param[0]), fy(param[1]), cx(param[2]), cy(param[3]), k1(param[4]), k2(param[5]), k3(param[6]), k4(param[7]){}
    
    template <typename T>
    bool operator()(const T* const camera, const T* const model, T* residual) const {
        T p[3];
        ceres::AngleAxisRotatePoint(camera, model, p);
        // camera[3,4,5] are the translation.
        p[0] += camera[3] ;//+ rtCamera[0];  ２つのカメラの中心を原点とする
        p[1] += camera[4] ;//+ rtCamera[1];
        p[2] += camera[5] ;//+ rtCamera[2];
        // Compute the center of distortion. The sign change comes from
        // the camera model that Noah Snavely's Bundler assumes, whereby
        // the camera coordinate system has a negative z axis.
        T a =  p[0] / p[2];
        T b =  p[1] / p[2];
        
        T r = sqrt(a * a + b * b);
        T t = atan(r);
        T d = t * ( T(1.0) + k1 * t * t + k2 * t * t * t * t + k3 * t * t * t * t * t * t + k4 * t * t * t * t * t * t * t * t);
        
        T xp = d / r * a;
        T yp = d / r * b;
        
        T pPx = T(fx) * xp + T(cx);
        T pPy = T(fy) * yp + T(cy);
        
        residual[0]= pPx - T(x);
        residual[1]= pPy - T(y);
        
        return true;
        //本当に拘束されているか検証必要　カメラの位置よりもモデル座標の拘束を。
    }
    
private:
    const double x, y;
    const double fx, fy, cx, cy;
    const double k1, k2, k3, k4;
};
//-----------------------------

class aruco_manager{
    Eigen::Matrix4d center_camera_L;
    aruco_manager(){
        center_camera_L << 1,0,0,-32,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1;
    }
    Eigen::Matrix4d getLcamera(Eigen::Matrix4d rs){
        Eigen::Matrix4d x;
        return x;
    }
    void drawSingleMarker(ofFbo &fbo){
        
    }
    void drawMarkers(ofFbo &fbo){
        
    }
    void stackMarkerPosition(){
        
    }
    void fishEyeCamera(){
        
    }
    
};
