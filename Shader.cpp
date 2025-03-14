#include "Shader.h"

Eigen::Matrix4f modelview;
Eigen::Matrix4f projection;
Eigen::Matrix4f viewport;

void set_modelview(Eigen::Vector3f eye_pos, Eigen::Vector3f look_at, Eigen::Vector3f up){
    Eigen::Matrix4f translation, ratation;
    translation << 1, 0, 0, -eye_pos[0],
                   0, 1, 0, -eye_pos[1],
                   0, 0, 1, -eye_pos[2],
                   0, 0, 0,  1;
    Eigen::Vector3f lxu = look_at.cross(up); 
    ratation <<  lxu[0],      lxu[1],      lxu[2],     0,
                 up[0],       up[1],       up[2],      0,
                -look_at[0], -look_at[1], -look_at[2], 0,
                 0,           0,           0,          1;
    modelview = ratation * translation;
}

void set_projection(float fov, float aspect_ratio, float n, float f){
    float l, r, t, b;
    t = tan(fov/2)*std::fabs(n);
    b = -t;
    r = aspect_ratio*t;
    l = -r;

    Eigen::Matrix4f M_scale, M_trans, M_ortho, M_persp2ortho;

    M_scale << 2/(r-l), 0,       0,       0,
               0,       2/(t-b), 0,       0,
               0,       0,       2/(n-f), 0,
               0,       0,       0,       1;
    M_trans << 1, 0, 0, -(r+l)/2,
               0, 1, 0, -(t+b)/2,
               0, 0, 1, -(n+f)/2,
               0, 0, 0, 1;
    M_ortho = M_scale*M_trans;

    M_persp2ortho << n, 0, 0,   0,
                     0, n, 0,   0,
                     0, 0, n+f, -n*f,
                     0, 0, 1,   0;
    projection = M_ortho*M_persp2ortho;
}

void set_viewport(int w, int h){
    viewport << w/2.0, 0,       0, w/2.0,
                  0,     h/2.0, 0, h/2.0,
                  0,     0,     1, 0,
                  0,     0,     0, 1;
}

Eigen::Vector4f toVec4(Eigen::Vector3f v, float w){
    return Eigen::Vector4f(v[0], v[1], v[2], w);
}

Vertex vertex_shader(const vertex_shader_payload& payload){
    Vertex v;
    v.coord = viewport*projection*modelview*payload.coord;
    //透视除法
    v.coord = v.coord / v.coord[3];
    
    v.viewspace_coord = (modelview*payload.coord).head<3>();
    v.normal = (modelview.inverse().transpose()*toVec4(payload.normal, 0.f)).head<3>().normalized();
    v.color = Eigen::Vector3f(79, 112, 156);
    v.tex_coord = payload.tex_coord;

    return v;
}

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload){
    Eigen::Vector3f color = (payload.normal.normalized() + Eigen::Vector3f(1.f, 1.f, 1.f)) / 2.f;
    return color*255;
}

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload){
    Light light{{1, 1, 1}, {50, 50, 50}};

    float u = payload.tex_coord[0] * payload.texture->width();
    float v = payload.tex_coord[1] * payload.texture->height();
    Eigen::Vector3f color = payload.texture->getColor(u, v);

    Eigen::Vector3f l = (modelview*toVec4(light.position, 0.f)).head<3>().normalized();
    float intensity = std::max(0.f, payload.normal.normalized().dot(l.normalized()));
    return color*intensity;
}

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload){
    Eigen::Vector3f ka = Eigen::Vector3f(1, 1, 1);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(150, 150, 150);

    Light light{{10, 10, 10}, {140, 140, 140}};

    Eigen::Vector3f amb_light = Eigen::Vector3f(10, 10, 10);

    Eigen::Vector3f point = payload.viewspace_coord;
    Eigen::Vector3f normal = payload.normal;

    //point位于viewspace中，所以light_pos和eye_pos也需要变换到viewspace中
    //eye_pos变换后就位于原点，在vertex_shader中，normal已经变换到viewspace中
    Eigen::Vector3f eye_pos = Eigen::Vector3f(0, 0, 0);
    Eigen::Vector3f light_pos = (modelview*toVec4(light.position, 1.f)).head<3>();

    Eigen::Vector3f l = light_pos - point;
    Eigen::Vector3f v = eye_pos - point;
    Eigen::Vector3f h = l + v;

    float r2 = l.dot(l);
    float p = 50;

    Eigen::Vector3f La = ka.cwiseProduct(amb_light);
    Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r2)*std::max(0.f, normal.normalized().dot(l.normalized()));
    Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r2)*std::pow(std::max(0.f, normal.normalized().dot(h.normalized())), p);

    Eigen::Vector3f color = La + Ld + Ls ;
    return color;
}

//object space normal mapping
Eigen::Vector3f object_nm_fragment_shader(const fragment_shader_payload& payload){
    Light light{{1, 1, 1}, {100, 100, 100}};

    float u = payload.tex_coord[0] * payload.normal_map->width();
    float v = payload.tex_coord[1] * payload.normal_map->height();
    Eigen::Vector3f col = payload.normal_map->getColor(u, v);

    Eigen::Vector3f normal;
    for(int i = 0; i < 3; i++){
        normal[i] = col[i] / 255.f * 2.f - 1.f;
    }

    float intensity = std::max(0.f, normal.normalized().dot(light.position.normalized()));

    u = payload.tex_coord[0] * payload.texture->width();
    v = payload.tex_coord[1] * payload.texture->height();
    Eigen::Vector3f color = payload.texture->getColor(u, v);

    return color*intensity;
}

//tangent space normal mapping
Eigen::Vector3f tangent_nm_fragment_shader(const fragment_shader_payload& payload){
    Light light{{1, 1, 1}, {100, 100, 100}};

    Eigen::Matrix3f AI = payload.A.inverse();

    Eigen::Vector3f t = (AI * payload.delta_u).normalized();
    Eigen::Vector3f n = payload.normal.normalized();
    Eigen::Vector3f b = (n.cross(t)).normalized();

    float x = t.dot(n);

    Eigen::Matrix3f TBN;
    TBN << t[0], b[0], n[0],
           t[1], b[1], n[1],
           t[2], b[2], n[2];
    
    float u = payload.tex_coord[0] * payload.normal_map->width();
    float v = payload.tex_coord[1] * payload.normal_map->height();
    Eigen::Vector3f col = payload.normal_map->getColor(u, v);

    Eigen::Vector3f normal;
    for(int i = 0; i < 3; i++){
        normal[i] = col[i] / 255.f * 2.f - 1.f;
    }

    normal = (TBN * normal).normalized();
    
    Eigen::Vector3f l = (modelview*toVec4(light.position, 0.f)).head<3>();
    float intensity = std::max(0.f, normal.normalized().dot(l.normalized()));

    u = payload.tex_coord[0] * payload.texture->width();
    v = payload.tex_coord[1] * payload.texture->height();
    Eigen::Vector3f color = payload.texture->getColor(u, v);

    return color*intensity;
}

Eigen::Vector3f composite_fragment_shader(const fragment_shader_payload& payload){
    float x = payload.tex_coord[0] * payload.texture->width();
    float y = payload.tex_coord[1] * payload.texture->height();

    Eigen::Vector3f ka = Eigen::Vector3f(1.2, 1.2, 1.2);
    Eigen::Vector3f kd = payload.texture->getColor(x, y);
    Eigen::Vector3f ks = Eigen::Vector3f(80, 80, 80);

    Light light{{10, 10, 10}, {240, 240, 240}};
    Eigen::Vector3f amb_light = Eigen::Vector3f(10, 10, 10);

    x = payload.tex_coord[0] * payload.normal_map->width();
    y = payload.tex_coord[1] * payload.normal_map->height();
    Eigen::Vector3f col = payload.normal_map->getColor(x, y);

    Eigen::Vector3f normal;
    for(int i = 0; i < 3; i++){
        normal[i] = col[i] / 255.f * 2.f - 1.f;
    }
    normal = (modelview.inverse().transpose()*toVec4(normal, 0.f)).head<3>().normalized();
    
    Eigen::Vector3f point = payload.viewspace_coord;
    //point位于viewspace中(经过modelview变换后的空间)，所以light_pos和eye_pos也需要进行变换
    //eye_pos变换后就位于原点，在vertex_shader中，normal已经变换到viewspace中
    Eigen::Vector3f eye_pos = Eigen::Vector3f(0, 0, 0);
    Eigen::Vector3f light_pos = (modelview*toVec4(light.position, 1.f)).head<3>();

    Eigen::Vector3f l = light_pos - point;
    Eigen::Vector3f v = eye_pos - point;
    Eigen::Vector3f h = l + v;

    float r2 = l.dot(l);
    float p = 100;

    Eigen::Vector3f La = ka.cwiseProduct(amb_light);
    Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r2)*std::max(0.f, normal.normalized().dot(l.normalized()));
    Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r2)*std::pow(std::max(0.f, normal.normalized().dot(h.normalized())), p) * 0.5;

    return La + Ld + Ls;
}