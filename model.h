#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include <eigen3/Eigen/Eigen>
#include "Triangle.h"

class Model{
private:
    std::vector<Eigen::Vector3f> coords;
    std::vector<Eigen::Vector3f> tex_coords;
    std::vector<Eigen::Vector3f> normals;
    std::vector<Triangle> triangles;
public:
    Model(const char *filename);
    ~Model();
    int nVertices();
    int nTriangles();
    Eigen::Vector3f getCoord(int i);
    Eigen::Vector3f getTexCoord(int i);
    Eigen::Vector3f getNormal(int i);
    std::vector<Triangle>* getTriangels();
};

#endif