#include "Triangle.h"

Triangle::Triangle(){
    coords[0] << 0.0, 0.0, 0.0, 1.0;
    coords[1] << 0.0, 0.0, 0.0, 1.0;
    coords[2] << 0.0, 0.0, 0.0, 1.0;

    color[0] << 0.0, 0.0, 0.0;
    color[1] << 0.0, 0.0, 0.0;
    color[2] << 0.0, 0.0, 0.0;

    tex_coords[0] << 0.0, 0.0;
    tex_coords[1] << 0.0, 0.0;
    tex_coords[2] << 0.0, 0.0;

    normals[0] << 0.0, 0.0, 0.0;
    normals[1] << 0.0, 0.0, 0.0;
    normals[2] << 0.0, 0.0, 0.0;
}

void Triangle::setVertex(int i, Eigen::Vector4f coord){
    coords[i] = coord;
}

void Triangle::setTexCoord(int i, Eigen::Vector2f tex_coord){
    tex_coords[i] = tex_coord;
}

void Triangle::setColor(int i, float r, float g, float b){
    color[i] = Eigen::Vector3f((float)r/255.0, (float)g/255.0, (float)b/255.0);
}

void Triangle::setNormal(int i, Eigen::Vector3f normal){
    normals[i] = normal;
}

