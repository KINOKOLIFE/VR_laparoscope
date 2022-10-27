#pragma once
#include "ofApp.h"
#include "sheared_stracture.h"

class objLoader{
public:
    ofMesh mesh;
    std::vector<ofMesh> mesh_;
    int meshCount;
    string path;
    ofxAssimpModelLoader model;
    bool set;
    bool show = false;
    string thum;
    void setup(string imagePath){
        this->path = imagePath;
    }
    objLoader(){
        
    }
    objLoader(string s){
        bool b = model.loadModel(s);
        if(b){
            std::cout<<"success"<<std::endl;
            model.setScaleNormalization(false);
            meshCount = model.getMeshCount();
            //mesh = model.getMesh(3);
            //setNormals(mesh);
            std::cout<<meshCount<<std::endl;
            for(int i = 0; i < meshCount; i++){
                ofMesh m = model.getMesh(i);
                for(size_t i = 0; i < m.getNumColors(); i++){
                    m.getColors()[i];// Use this to get the current color
                    m.setColor(i, ofFloatColor(255,0,0));// pass which ever ofFloatColor you want to.
                }
                setNormals(m);
                mesh_.push_back(m);
            }
            set = true;
            this->path = s;
            char delim = '/';
            vector<string>strs = split(s,delim);
            thum = strs[strs.size() - 1];
        }else{
            std::cout<<"fault"<<std::endl;
            set = false;
        }
        
        
    }
    
    void setNormals(ofMesh &mesh ){
    //The number of the vertices
        int nV = mesh.getNumVertices();
    //The number of the triangles
        int nT = mesh.getNumIndices() / 3;
        vector<ofPoint> norm( nV ); //Array for the normals
        vector<glm::vec3> norm1(nV);
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
        for (int t=0; t<nT; t++) {
            //Get indices of the triangle t
            int i1 = mesh.getIndex( 3 * t );
            int i2 = mesh.getIndex( 3 * t + 1 );
            int i3 = mesh.getIndex( 3 * t + 2 );
            //Get vertices of the triangle
            const ofPoint &v1 = mesh.getVertex( i1 );
            const ofPoint &v2 = mesh.getVertex( i2 );
            const ofPoint &v3 = mesh.getVertex( i3 );
            //Compute the triangle's normal
            ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
            //Accumulate it to norm array for i1, i2, i3
            norm1[ i1 ] += dir;
            norm1[ i2 ] += dir;
            norm1[ i3 ] += dir;
        }

        //Normalize the normal's length
        for (int i=0; i<nV; i++) {
            norm1[i] = glm::normalize(norm1[i]);
        }
        //Set the normals to mesh
        mesh.clearNormals();
        //mesh.addNormals( norm );
        mesh.addNormals(norm1);
  
    }
    vector<string> split(const string &s, char delim) {
        vector<string> elems;
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim)) {
        if (!item.empty()) {
                elems.push_back(item);
            }
        }
        return elems;
    }

};
