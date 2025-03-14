#ifndef _SHADER_H_
#define _SHADER_H_

#include <eigen3/Eigen/Eigen>
#include "Texture.h"

struct Vertex{
    Eigen::Vector4f coord;
    Eigen::Vector2f tex_coord;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector3f viewspace_coord;
};

struct Light{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

struct vertex_shader_payload{
    vertex_shader_payload(const Eigen::Vector4f& coo, const Eigen::Vector2f& tex_coo,
                          const Eigen::Vector3f& col, const Eigen::Vector3f& nor) :  
    coord(coo), tex_coord(tex_coo), color(col), normal(nor) {}

    Eigen::Vector4f coord;
    Eigen::Vector2f tex_coord;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
};

struct fragment_shader_payload{
    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor,
                            const Eigen::Vector2f& tc, const Eigen::Vector3f& vc,
                            const Eigen::Vector3f& ep, const Eigen::Matrix3f& a,
                            const Eigen::Vector3f& du, Texture* tex, Texture* nm) : 
    color(col), normal(nor), tex_coord(tc), eye_pos(ep), viewspace_coord(vc),  A(a),
    delta_u(du), texture(tex), normal_map(nm) {}

    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f tex_coord;
    Eigen::Vector3f eye_pos;
    Eigen::Vector3f viewspace_coord;
    Eigen::Matrix3f A;
    Eigen::Vector3f delta_u;
    Texture* texture;
    Texture* normal_map;
};

void set_modelview(Eigen::Vector3f eye_pos, Eigen::Vector3f look_at, Eigen::Vector3f up);
void set_projection(float fov, float aspect_ratio, float n, float f);
void set_viewport(int w, int h);

Eigen::Vector4f toVec4(Eigen::Vector3f v, float w = 1.0f);

Vertex vertex_shader(const vertex_shader_payload& payload);

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f object_nm_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f tangent_nm_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f composite_fragment_shader(const fragment_shader_payload& payload);

#endif