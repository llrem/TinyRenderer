#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "model.h"

Model::Model(const char *filename){
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if(in.fail()) return; 
    std::string line, strash;
    char trash;
    int index, itrash;
    while(!in.eof()){
        std::getline(in, line);
        std::istringstream iss(line);
        if(!line.compare(0, 2, "v ")){
            Eigen::Vector3f v;
            iss >> trash;
            for(int i = 0; i < 3; i++){
                iss >> v[i];
            }
            coords.push_back(v);
        }else if(!line.compare(0, 3, "vt ")){
            Eigen::Vector3f vt;
            iss >> strash;
            for(int i = 0; i < 3; i++){
                iss >> vt[i];
            }
            tex_coords.push_back(vt);
        }else if(!line.compare(0, 3, "vn ")){
            Eigen::Vector3f vn;
            iss >> strash;
            for(int i = 0; i < 3; i++){
                iss >> vn[i];
            }
            normals.push_back(vn);
        }else if(!line.compare(0, 2, "f ")){
            Triangle t;
            int temp;
            iss >> trash;
            for(int i = 0; i < 3; i++){
                iss >> temp >> trash;
                Eigen::Vector3f coord = getCoord(--temp);
                t.setVertex(i, Eigen::Vector4f(coord[0], coord[1], coord[2], 1.0));
                iss >> temp >> trash;
                Eigen::Vector3f tex_coord = getTexCoord(--temp);
                t.setTexCoord(i, Eigen::Vector2f(tex_coord[0], tex_coord[1]));
                iss >> temp;
                t.setNormal(i, getNormal(--temp));
            }
            triangles.push_back(t);
        }
    }
}

Model::~Model(){

}

int Model::nVertices(){
    return coords.size();
}

int Model::nTriangles(){
    return triangles.size();
}

Eigen::Vector3f Model::getCoord(int i){
    return coords[i];
}

Eigen::Vector3f Model::getTexCoord(int i){
    return tex_coords[i];
}

Eigen::Vector3f Model::getNormal(int i){
    return normals[i];
}

std::vector<Triangle>* Model::getTriangels(){
    return &triangles;
}