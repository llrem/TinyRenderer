#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <eigen3/Eigen/Eigen>
#include "tgaimage.h"

class Texture{
private:
    TGAImage tex;
public:
    Texture(const std::string& file_path){
        tex.read_tga_file(file_path.c_str());
        tex.flip_vertically();
    }

    int width(){
        return tex.get_width();
    }

    int height(){
        return tex.get_height();
    }

    Eigen::Vector3f getColor(float u, float v){
        TGAColor color = tex.get(u, v);
        return Eigen::Vector3f(color.bgra[2], color.bgra[1], color.bgra[0]);
    }
};

#endif