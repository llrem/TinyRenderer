#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <eigen3/Eigen/Eigen>

class Triangle{
public:
    Eigen::Vector4f coords[3];
    Eigen::Vector2f tex_coords[3];
    Eigen::Vector3f color[3];
    Eigen::Vector3f normals[3];

    Triangle();
    void setVertex(int i, Eigen::Vector4f coord);
    void setTexCoord(int i, Eigen::Vector2f tex_coord);
    void setColor(int i, float r, float g, float b);
    void setNormal(int i, Eigen::Vector3f normal);
};

#endif