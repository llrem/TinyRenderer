#include "Rasterizer.h"

Rasterizer::Rasterizer(int wid, int hei){
    width = wid, height = hei;
    image = TGAImage(width, height, TGAImage::RGB);
    
    z_buffer = new float[width * height];
    for(int i = 0; i < width * height; i++){
        z_buffer[i] = -std::numeric_limits<float>::max();
    }
}

Eigen::Vector3f Rasterizer::barycentric2D(Eigen::Vector2f p, Eigen::Vector4f* v){
    float beta  = ((v[0].y()-v[2].y())*p.x()+(v[2].x()-v[0].x())*p.y()+v[0].x()*v[2].y()-v[2].x()*v[0].y())/((v[0].y()-v[2].y())*v[1].x()+(v[2].x()-v[0].x())*v[1].y()+v[0].x()*v[2].y()-v[2].x()*v[0].y());
    float gamma = ((v[0].y()-v[1].y())*p.x()+(v[1].x()-v[0].x())*p.y()+v[0].x()*v[1].y()-v[1].x()*v[0].y())/((v[0].y()-v[1].y())*v[2].x()+(v[1].x()-v[0].x())*v[2].y()+v[0].x()*v[1].y()-v[1].x()*v[0].y());
    float alpha = 1 - beta - gamma;
    return Eigen::Vector3f(alpha, beta, gamma);
}

Eigen::Vector3f Rasterizer::interpolate(float alpha, float beta, float gamma, Eigen::Vector3f& v1, Eigen::Vector3f& v2, Eigen::Vector3f& v3, float weight){
    return (alpha * v1 + beta * v2 + gamma * v3) / weight;
}

Eigen::Vector2f Rasterizer::interpolate(float alpha, float beta, float gamma, Eigen::Vector2f& v1, Eigen::Vector2f& v2, Eigen::Vector2f& v3, float weight){
    return (alpha * v1 + beta * v2 + gamma * v3) / weight;
}

void Rasterizer::set_eye_pos(Eigen::Vector3f e_pos){
    eye_pos = e_pos;
}

void Rasterizer::set_texture(Texture& tex){
    texture = &tex;
}

void Rasterizer::set_normal_map(Texture& nm){
    normal_map = &nm;
}

void Rasterizer::set_pixel(int i, int j, Eigen::Vector3f color){
    image.set(i, j, TGAColor(color[0], color[1], color[2], 255));
}

void Rasterizer::set_vertex_shader(std::function<Vertex(vertex_shader_payload)> vert_shader){
    vertex_shader = vert_shader;
}

void Rasterizer::set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader){
    fragment_shader = frag_shader;
}

void Rasterizer::rasterize_triangle(Triangle& t,  Eigen::Vector3f* viewspace_coord){
    int bboxmin[2] = {width-1, height-1}, bboxmax[2] = {0, 0}; 
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 2; j++){
            bboxmin[j] = std::min(bboxmin[j], static_cast<int>(t.coords[i][j]));
            bboxmax[j] = std::max(bboxmax[j], static_cast<int>(t.coords[i][j]));
        }
    }
    int min_x = std::max(bboxmin[0], 0);
    int max_x = std::min(bboxmax[0], width-1);
    int min_y = std::max(bboxmin[1], 0);
    int max_y = std::min(bboxmax[1], height-1);

    for(int i = min_x; i <= max_x; i++){
        for(int j = min_y; j <= max_y; j++){
            Eigen::Vector3f bcc = barycentric2D({i, j}, t.coords);
            if(bcc[0] >= 0 && bcc[1] >= 0 && bcc[2] >= 0){
                float z = bcc[0]*t.coords[0].z() + bcc[1]*t.coords[1].z() + bcc[2]*t.coords[2].z();
                if(z > z_buffer[j*width+i]){
                    z_buffer[j*width+i] = z;

                    Eigen::Vector3f interpolated_normal = interpolate(bcc[0], bcc[1], bcc[2], t.normals[0], t.normals[1], t.normals[2], 1);
                    Eigen::Vector2f interpolated_texcoords = interpolate(bcc[0], bcc[1], bcc[2], t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], 1);
                    Eigen::Vector3f interpolated_color = interpolate(bcc[0], bcc[1], bcc[2], t.color[0], t.color[1], t.color[2], 1);
                    Eigen::Vector3f interpolated_viewspace_coord = interpolate(bcc[0], bcc[1], bcc[2], viewspace_coord[0], viewspace_coord[1], viewspace_coord[2], 1);

                    //用于tangent normal mapping中，求切线空间的t向量
                    Eigen::Vector3f p1 = viewspace_coord[1] - viewspace_coord[0];
                    Eigen::Vector3f p2 = viewspace_coord[2] - viewspace_coord[0];
                    Eigen::Vector3f n = interpolated_normal.normalized();
                    Eigen::Matrix3f A;
                    A << p1[0], p1[1], p1[2],
                         p2[0], p2[1], p2[2],
                         n[0],  n[1],  n[2];

                    float delta_u1 = t.tex_coords[1][0] - t.tex_coords[0][0];
                    float delta_u2 = t.tex_coords[2][0] - t.tex_coords[0][0];

                    Eigen::Vector3f delta_u(delta_u1, delta_u2, 0);

                    fragment_shader_payload payload(interpolated_color, interpolated_normal.normalized(), interpolated_texcoords,
                                                    interpolated_viewspace_coord, eye_pos, A, delta_u, texture, normal_map);

                    Eigen::Vector3f color = fragment_shader(payload);
                    set_pixel(i, j, color);
                }
            }
        }
    }
}

void Rasterizer::draw(std::vector<Triangle>* TriangleList){
    for(auto& t : *TriangleList){
        Eigen::Vector3f viewspace_coord[3];
        for(int j = 0; j < 3; j++){
            vertex_shader_payload payload(t.coords[j], t.tex_coords[j], t.color[j], t.normals[j]);
            Vertex v = vertex_shader(payload);

            t.coords[j] = v.coord;
            t.tex_coords[j] = v.tex_coord;
            t.color[j] = v.color;
            t.normals[j] = v.normal;
            viewspace_coord[j] = v.viewspace_coord;
        }
        rasterize_triangle(t, viewspace_coord);
    }
    image.flip_vertically();
    image.write_tga_file("output1.tga");
}