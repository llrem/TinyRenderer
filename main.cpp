#include <eigen3/Eigen/Eigen>
#include "model.h"
#include "Rasterizer.h"

#define PI 3.1415926
const int width = 800;
const int height = 800;

Eigen::Vector3f eye_pos(1, 1, 3);
Eigen::Vector3f look_at = {-1, -1, -3};
Eigen::Vector3f up = {-1, 10, -3};

int main(int argc, char** argv){
    Model *model = NULL;
    if(argc == 2){
        model = new Model(argv[1]);
    }else{
        model = new Model("../obj/african_head.obj");
    }
    
    Rasterizer r(width, height);

    Texture texture("../obj/african_head_texture.tga");
    Texture normal_map("../obj/african_head_nm.tga");
    r.set_texture(texture);
    r.set_normal_map(normal_map);
    
    r.set_eye_pos(eye_pos);

    set_modelview(eye_pos, look_at.normalized(), up.normalized());
    set_projection(std::atan(1.0/(eye_pos[2]-1))*2, 1.0, -1.0, -100.0);
    set_viewport(width, height);

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = composite_fragment_shader;
    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);
    
    r.draw(model->getTriangels());    
    return 0;
}