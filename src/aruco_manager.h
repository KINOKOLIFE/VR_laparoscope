#pragma once
#include "ofApp.h"
#include "graphics.h"
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

static void my_multMat(Eigen::Matrix4d m, std::vector<cv::Vec3d> src, std::vector<cv::Vec3d> &dist){
    for( auto p : src){
        Eigen::Vector4d vs = Eigen::Vector4d(p(0), p(1), p(2), 1.0);
        Eigen::Vector4d vd = m * vs;
        cv::Vec3d d = cv::Vec3d(vd.x(), vd.y(), vd.z());
        dist.push_back(d);
    }
}

struct icp_f{
    icp_f(cv::Vec3d refp, cv::Vec3d newp)
    :ref_x(refp(0)), ref_y(refp(1)), ref_z(refp(2)), new_x(newp(0)), new_y(newp(1)), new_z(newp(2)){}
    template <typename T>
    bool operator()(const T* const result, T* residual) const {
        T p[3];
        T point[3];
        point[0] = T(ref_x);
        point[1] = T(ref_y);
        point[2] = T(ref_z);
        ceres::AngleAxisRotatePoint(result, point, p);
        p[0] += result[3] ;//+ rtCamera[0];
        p[1] += result[4] ;//+ rtCamera[1];
        p[2] += result[5] ;//+ rtCamera[2];
        residual[0]=  ( p[0] - T(new_x) ) * ( p[0] - T(new_x) )
                    + ( p[1] - T(new_y) ) * ( p[1] - T(new_y) )
                    + ( p[2] - T(new_z) ) * ( p[2] - T(new_z) );
        return true;
    }
private:
    const double new_x, new_y, new_z;
    const double ref_x, ref_y, ref_z;
};
//----simple fitは未使用
struct simpleFit{
    simpleFit(double observed_x, double observed_y, double* param, double* pt)
    :x(observed_x), y(observed_y), fx(param[0]), fy(param[1]), cx(param[2]), cy(param[3]), k1(param[4]), k2(param[5]), k3(param[6]), k4(param[7]), px(pt[0]), py(pt[1]), pz(pt[2]){}
    
    template <typename T>
    bool operator()(const T* const camera, T* residual) const {
        T p[3];
        T point[3];
        point[0] = T(px);
        point[1] = T(py);
        point[2] = T(pz);
        ceres::AngleAxisRotatePoint(camera, point, p);
        // camera[3,4,5] are the translation.
        p[0] += camera[3] ;//+ rtCamera[0];
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
    }
    
private:
    const double x, y;
    const double fx, fy, cx, cy;
    const double px, py, pz;
    const double k1, k2, k3, k4;
};



struct pointFit_BA{
    pointFit_BA(double observed_x, double observed_y, double* param)
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

class BA_point{
public:
    double rms;
    struct observation{
        cv::Vec3d rvec;//camera inverse matrix
        cv::Vec3d tvec;
        std::vector<cv::Vec3d> points;//marker
        std::vector<int>ids;
        std::vector<cv::Vec2d> centers;
    };
    struct single_observation{
        
        cv::Vec3d caemra_inverse_rvec;
        
        cv::Vec3d tvec;
        
        cv::Vec3d marker_tvecs;
        int ids;
        cv::Vec2d projectpoints;
    };
    std::vector<observation> observations;
    std::vector<single_observation> single_observations;
    void calc_center(vector<cv::Point2f> &corners, cv::Vec2d &o){
        //(x1,y1)-(x3,y3)
        //(x2,y2)-(x4,y4)
        double x1 = (double)corners[0].x;
        double y1 = (double)corners[0].y;
        double x2 = (double)corners[1].x;
        double y2 = (double)corners[1].y;
        double x3 = (double)corners[2].x;
        double y3 = (double)corners[2].y;
        double x4 = (double)corners[3].x;
        double y4 = (double)corners[3].y;
        double s1 = ((x4 - x2) * (y1 - y2) - (y4 - y2) * (x1 - x2)) / 2;
        double s2 = ((x4 - x2) * (y2 - y3) - (y4 - y2) * (x2 - x3)) / 2;
        double x = x1 + (x3 - x1) * s1 / (s1 + s2);
        double y = y1 + (y3 - y1) * s1 / (s1 + s2);
        o = cv::Vec2d(x, y);
    }
    double *camera_inverse_mat;
    double camparam[8];
    double solution[3];
    
    ~BA_point(){
        delete[] camera_inverse_mat;
    }
    BA_point(){
        
    }
    
    BA_point(cv::Mat _intrincs, cv::Mat _distortion){
        camera_inverse_mat = new double[ 200 * 6 ];
        
        camparam[4] = _distortion.at<double>(0,0);
        camparam[5] = _distortion.at<double>(0,1);
        camparam[6] = _distortion.at<double>(0,2);
        camparam[7] = _distortion.at<double>(0,3);
        
        camparam[0] = _intrincs.at<double>(0,0);
        camparam[1] = _intrincs.at<double>(1,1);
        camparam[2] = _intrincs.at<double>(0,2);
        camparam[3] = _intrincs.at<double>(1,2);
    }
    
    void setData(std::vector<single_observation> &_single_observations){
        double average_x = 0, average_y = 0, average_z = 0;
        for( auto o: _single_observations){
            average_x += o.marker_tvecs(0);
            average_y += o.marker_tvecs(1);
            average_z += o.marker_tvecs(2);
        }
        average_x /= _single_observations.size();
        average_y /= _single_observations.size();
        average_z /= _single_observations.size();
        double average_norm = 0;
        for( auto o: _single_observations){
            average_norm += sqrt(
            (average_x - o.marker_tvecs(0)) * (average_x - o.marker_tvecs(0)) +
            (average_y - o.marker_tvecs(1)) * (average_y - o.marker_tvecs(1)) +
            (average_z - o.marker_tvecs(2)) * (average_z - o.marker_tvecs(2)));
        }
        average_norm /= _single_observations.size();
        double sigma_ = 0;
        for( auto o: _single_observations){
            sigma_ += pow(
                         ( sqrt(
                            (average_x - o.marker_tvecs(0)) * (average_x - o.marker_tvecs(0)) +
                            (average_y - o.marker_tvecs(1)) * (average_y - o.marker_tvecs(1)) +
                            (average_z - o.marker_tvecs(2)) * (average_z - o.marker_tvecs(2)))
                          - average_norm), 2);
        }
        sigma_ /=  _single_observations.size();
        double sigma = sqrt(sigma_);
        
        for( auto o: _single_observations){
            double tscore = (sqrt(
                                 (average_x - o.marker_tvecs(0)) * (average_x - o.marker_tvecs(0)) +
                                 (average_y - o.marker_tvecs(1)) * (average_y - o.marker_tvecs(1)) +
                                 (average_z - o.marker_tvecs(2)) * (average_z - o.marker_tvecs(2)))
            - average_norm) * 10.0 / sigma + 50;
           
        }
        
        int num = _single_observations.size();
        for (int i = 0; i < num; i++) {
            camera_inverse_mat[ i * 6     ] = _single_observations[i].caemra_inverse_rvec(0);
            camera_inverse_mat[ i * 6 + 1 ] = _single_observations[i].caemra_inverse_rvec(1);
            camera_inverse_mat[ i * 6 + 2 ] = _single_observations[i].caemra_inverse_rvec(2);
            
            camera_inverse_mat[ i * 6 + 3 ] = _single_observations[i].tvec(0);
            camera_inverse_mat[ i * 6 + 4 ] = _single_observations[i].tvec(1);
            camera_inverse_mat[ i * 6 + 5 ] = _single_observations[i].tvec(2);
            
        }
        solution[0] = average_x; solution[1] = average_y; solution[2] = average_z;
        ceres::Problem problem;
        for(int i = 0; i < num; i++){
            
            double x = _single_observations[i].projectpoints(0);
            double y = _single_observations[i].projectpoints(1);
            
            ceres::CostFunction* cost_function=new ceres::AutoDiffCostFunction<pointFit_BA, 2, 6, 3>( new pointFit_BA(x, y, camparam));
            problem.AddResidualBlock(cost_function, nullptr,  camera_inverse_mat + i * 6, solution);
        }
        ceres::Solver::Options options;//最適化のオプション設定用構造体
        options.linear_solver_type=ceres::DENSE_SCHUR;
        options.minimizer_progress_to_stdout=false;
        ceres::Solver::Summary summary;//最適化の結果を格納するよう構造体
        Solve(options,&problem,&summary);//最適化の実
        //std::cout<<solution[0]<<" "<<solution[1]<<" "<<solution[2]<<std::endl;
    }
    void setData(std::vector<observation> &_observations, std::vector<int> &_ids, std::vector<cv::Vec3d> &_tvecs){
        std::cout<<"observation num"<<_observations.size()<<std::endl;
        int num_observatios = _observations.size();
        
        int num_observed_points = 0;
        for( auto o:_observations ){
            num_observed_points += o.ids.size();
        }
        std::cout<<"observedpoints_num"<<num_observed_points<<std::endl;
        double *mvm;
        //double *observations;
        double *points;
        mvm = new double[_observations.size() * 6 ];
        for(int i = 0; i < _observations.size(); i++ ){
            mvm[ i * 6 ] = _observations[i].rvec(0);
            mvm[ i * 6 + 1 ] = _observations[i].rvec(1);
            mvm[ i * 6 + 2 ] = _observations[i].rvec(2);
            mvm[ i * 6 + 3 ] = _observations[i].tvec(0);
            mvm[ i * 6 + 4 ] = _observations[i].tvec(1);
            mvm[ i * 6 + 5 ] = _observations[i].tvec(2);
     
        }
        //observations = new double[num_observed_points];
        //calc average for intial solution
        std::map<std::string, int> org;
        std::map<int, double> center_ave_x;
        std::map<int, double> center_ave_y;
        std::map<int, double> center_ave_z;
        std::map<int, int> center_points_num;
        for(auto o: _observations){
            for(int i = 0; i < o.ids.size(); i++ ){
                center_ave_x[o.ids[i]] += o.points[i](0) ;
                center_ave_y[o.ids[i]] += o.points[i](1) ;
                center_ave_z[o.ids[i]] += o.points[i](2) ;
                center_points_num[o.ids[i]] ++;
            }
        }
        
        for(auto itr = center_points_num.begin(); itr != center_points_num.end(); ++itr) {
                std::cout << "key = " << itr->first << ", val = " << itr->second << "\n";
            center_ave_x[itr->first]  = center_ave_x[itr->first] / itr->second;
            center_ave_y[itr->first] = center_ave_y[itr->first] / itr->second;
            center_ave_z[itr->first] = center_ave_z[itr->first] / itr->second;
        }
      
        int points_num;
        int num_points = center_points_num.size();
        points = new double[ num_points * 3 ];
        int itr_array = 0;
        std::map<int, int> id_order;
        for(auto itr = center_points_num.begin(); itr != center_points_num.end(); ++itr) {
            std::cout << "key = " << itr->first <<" "<< center_ave_x[itr->first] <<" "<< center_ave_y[itr->first]<<" "<< center_ave_z[itr->first] << std::endl;;
            points[ itr_array * 3     ] = center_ave_x[itr->first];
            points[ itr_array * 3 + 1 ] = center_ave_y[itr->first];
            points[ itr_array * 3 + 2 ] = center_ave_z[itr->first];
            id_order[ itr->first ] = itr_array;
            itr_array ++;
            
        }
        ceres::Problem problem;
        for(int i = 0; i < _observations.size(); i++ ){
            for(int j = 0; j < _observations[i].ids.size(); j++ ){
                int order = id_order[_observations[i].ids[j]];
                
                double xx = _observations[i].centers[j](0);
                double yy = _observations[i].centers[j](1);
                ceres::CostFunction* cost_function=new ceres::AutoDiffCostFunction<pointFit_BA, 2, 6, 3>( new pointFit_BA(xx, yy, camparam));
                problem.AddResidualBlock(cost_function, nullptr,  mvm + i * 6, points + order * 3);
                
            }
        }
        ceres::Solver::Options options;//最適化のオプション設定用構造体
        options.linear_solver_type = ceres::DENSE_SCHUR;
        options.minimizer_progress_to_stdout = false;
        ceres::Solver::Summary summary;//最適化の結果を格納するよう構造体
        Solve(options,&problem,&summary);//最適化の実
        //std::cout << summary.FullReport() << "\n";
        std::cout << summary.final_cost <<endl;
        for( int i = 0; i < num_points; i++ ){
           // std::cout<<points[i * 3]<<" "<<points[i * 3 + 1]<<" "<<points[i * 3 + 2]<<std::endl;
            _tvecs.push_back(cv::Vec3d(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]));
    
        }
        for(auto itr = id_order.begin(); itr != id_order.end(); ++itr) {
            _ids.push_back(itr->first);
        }
        rms = summary.final_cost;
        delete mvm;
        delete points;
    }
};

class my_icp{
public:
    my_icp(){
    }
    cv::Vec3d average(std::vector<cv::Vec3d> &points){
        cv::Vec3d ave = cv::Vec3d(0,0,0);
        for( auto p : points){
            ave += p;
        }
        int num = points.size();
        ave(0) = ave(0) / num;
        ave(1) = ave(1) / num;
        ave(2) = ave(2) / num;
        return ave;
    }
    void exec(std::vector<int> &ref_ids, std::vector<cv::Vec3d> &ref_points, std::vector<int> &new_ids, std::vector<cv::Vec3d> &new_points, cv::Vec3d &rvec, cv::Vec3d &tvec){
        cv::Vec3d ref_ave = average(ref_points);
        cv::Vec3d new_ave = average(new_points);
        cv::Vec3d def_ave = new_ave - ref_ave;
        double result[6];
        result[3] = def_ave(0);
        result[4] = def_ave(1);
        result[5] = def_ave(2);
        result[0] = 0;
        result[1] = 0;
        result[2] = 1.0;
        std::map<int, int> id_dic;
        for(int i = 0; i < ref_ids.size(); i++){
            id_dic[ ref_ids[i] ] = i;
        }
        ceres::Problem problem;
        for( int i = 0; i < new_ids.size(); i++){
            auto itr = id_dic.find(new_ids[i]);
            if( itr != id_dic.end() ) {
                int h = itr->second ;
                ceres::CostFunction* cost_function=new ceres::AutoDiffCostFunction<icp_f, 1, 6>( new icp_f(ref_points[h], new_points[i]));
                problem.AddResidualBlock(cost_function, nullptr,  result);
            }
        }
        ceres::Solver::Options options;//最適化のオプション設定用構造体
        options.linear_solver_type=ceres::DENSE_SCHUR;
        options.minimizer_progress_to_stdout=false;//最適化の結果を標準出力に表示する。
        ceres::Solver::Summary summary;//最適化の結果を格納するよう構造体
        Solve(options, &problem, &summary);
        rvec(0) = result[0]; rvec(1) = result[1]; rvec(2) = result[2];
        tvec(0) = result[3]; tvec(1) = result[4]; tvec(2) = result[5];
        
    }
};

struct markerstack{
    int id;
    std::vector<cv::Vec3d> tvecs;
};
//-----------------------------------
class aruco_manager{
public:
    int num_observations1;
    int num_observations2;
    bool capture_marker;
    bool adjust_marker;
    std::vector<cv::Vec3d> solutions_points;
    std::vector<int> solutions_id;
    vector<BA_point::observation>observations;
    vector<BA_point::single_observation>single_observations;
    bool capture_mode;
    vector<markerstack> markerarray;
    Eigen::Matrix4d center_camera_L;
    
    BA_point *ba;
    my_icp myicp;
    vector<cv::Vec3d> ref_points;
    vector<int> ref_ids;
    //--------
    aruco_manager(){
        center_camera_L << 1,0,0,-32,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1;
    }
    void calc_center(vector<cv::Point2f> &corners, cv::Vec2d &o){
        //(x1,y1)-(x3,y3)
        //(x2,y2)-(x4,y4)
        double x1 = (double)corners[0].x;
        double y1 = (double)corners[0].y;
        double x2 = (double)corners[1].x;
        double y2 = (double)corners[1].y;
        double x3 = (double)corners[2].x;
        double y3 = (double)corners[2].y;
        double x4 = (double)corners[3].x;
        double y4 = (double)corners[3].y;
        double s1 = ((x4 - x2) * (y1 - y2) - (y4 - y2) * (x1 - x2)) / 2;
        double s2 = ((x4 - x2) * (y2 - y3) - (y4 - y2) * (x2 - x3)) / 2;
        double x = x1 + (x3 - x1) * s1 / (s1 + s2);
        double y = y1 + (y3 - y1) * s1 / (s1 + s2);
        o = cv::Vec2d(x, y);
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
    void draw(ofFbo &fbo, ofEasyCam &cam){
        fbo.begin();
        cam.begin();
        for( auto m : markerarray ){
            for( int i = 0; i < m.tvecs.size(); i++){
                ofSetColor(ofColor::fromHsb( ( m.id % 5 ) * 20, 255, 255), 255 - i * 5);
                ofFill();
                ofDrawSphere(m.tvecs[i](0), m.tvecs[i](1), m.tvecs[i](2), 2);
            }
        }
        cam.end();
        fbo.end();
    }
    void update(vector<int> &markerIdsL ,vector<cv::Vec3d> &tvecs, vector<cv::Vec3d> &rvecs, vector<vector<cv::Point2f>> &markerCornersL, Eigen::Matrix4d realsense){
        bool flag = false;
        for( int j = 0; j < markerIdsL.size(); j++){
            for( int i = 0; i < markerarray.size(); i++){
                if( markerarray[i].id == markerIdsL[j] ){
                    markerarray[i].tvecs.push_back(tvecs[j]);
                    flag = true;
                }
                if( markerarray[i].tvecs.size() > 20){
                    markerarray[i].tvecs.erase(markerarray[i].tvecs.begin(), markerarray[i].tvecs.begin() + 1);
                }
            }
            if(!flag ){
                markerstack mks;
                mks.id = markerIdsL[j];
                mks.tvecs.push_back(tvecs[j]);
                markerarray.push_back( mks );
            }
        }
   
        if( capture_marker){
            std::vector<cv::Vec3d> points;//marker
            std::vector<int>ids;
            std::vector<cv::Vec2d> centers;
            std::vector<cv::Vec3d>tvecs_;
            std::vector<cv::Vec2d> projectpointcenters;
            Eigen::Matrix4d Lcamera = realsense * center_camera_L * rotx(180);
            Eigen::Matrix4d m1 = realsense * center_camera_L * rotx(180);
            Eigen::Matrix4d m = Lcamera.inverse();
            cv::Vec3d ci_rvec;
            cv::Vec3d ci_tvec;
            MatToVecs(ci_rvec, ci_tvec, m);
          
            for( int i = 0; i < markerIdsL.size(); i++){
                Eigen::Matrix4d m1 = VecsToMat(rvecs[i], tvecs[i]);
                Eigen::Matrix4d m2 =  Lcamera * m1;
                //draw_marker(m2, 26, ofColor(255), ofColor(255), perspective, easycam);
               
                cv::Vec3d rvec;
                cv::Vec3d tvec;
                MatToVecs(rvec,tvec, m2);
                
                cv::Vec2d center;
                calc_center(markerCornersL[i], center);
                double markerList[6] = {100, 101, 102, 103, 104, 105};
                for(int n = 0; n < 6; n++ ){
                    if( markerIdsL[i] == markerList[n] ){
                        points.push_back(tvec);
                        ids.push_back(markerIdsL[i]);
                        centers.push_back(center);
                    }
                }
            }
            if( ids.size() > 0 && capture_mode){
                observations.push_back(BA_point::observation{ci_rvec, ci_tvec, points, ids, centers});
            }
            num_observations1 = observations.size();
            if( observations.size() == 100){ // execute marker position capture
                solutions_id.clear();
                solutions_id.shrink_to_fit();
                solutions_points.clear();
                solutions_points.shrink_to_fit();
                ba->setData(observations, solutions_id, solutions_points);
                capture_mode = false;
                if(ba->rms < 30){
                    ofJson j;
                    ofJson pt;
                    ofJson order;
                    for ( int i = 0; i < solutions_id.size(); i++){
                        order[i] =  solutions_id[i];
                    }
                    j["ids"].push_back(order);
                    for ( int i = 0; i < solutions_id.size(); i++){
                        pt[to_string(solutions_id[i])] = {solutions_points[i](0),solutions_points[i](1),solutions_points[i](2)};
                    }
                    j["pt"].push_back(pt);
                    ofSaveJson("marker_point.json", j);
                }
                num_observations1 = 0;
            }
        }
        
    }
    
};
