#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include <eigen3/Eigen/Eigen>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include "Triangle.h"
#include "Texture.h"
#include "Shader.h"
#include "tgaimage.h"

class Rasterizer{
private:
    int width, height;
    TGAImage image;

    Texture* texture;
    Texture* normal_map;

    float* z_buffer;

    Eigen::Vector3f eye_pos;

    std::function<Vertex(vertex_shader_payload)> vertex_shader;
    std::function<Eigen::Vector3f(fragment_shader_payload)> fragment_shader;

    void rasterize_triangle(Triangle& t, Eigen::Vector3f* view_pos);

public:
    Rasterizer(int wid, int hei);

    Eigen::Vector3f barycentric2D(Eigen::Vector2f p, Eigen::Vector4f* v);
    Eigen::Vector3f interpolate(float alpha, float beta, float gamma, Eigen::Vector3f& v1, Eigen::Vector3f& v2, Eigen::Vector3f& v3, float weight);
    Eigen::Vector2f interpolate(float alpha, float beta, float gamma, Eigen::Vector2f& v1, Eigen::Vector2f& v2, Eigen::Vector2f& v3, float weight);

    void set_eye_pos(Eigen::Vector3f eye_pos);
    void set_texture(Texture& texture);
    void set_normal_map(Texture& normal_map);
    void set_pixel(int i, int j, Eigen::Vector3f color);

    void set_vertex_shader(std::function<Vertex(vertex_shader_payload)> vertex_shader);
    void set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> fragment_shader);

    void draw(std::vector<Triangle>* TriangleList);
};

#endif