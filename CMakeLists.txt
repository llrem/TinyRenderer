cmake_minimum_required(VERSION 3.0)

project(rasterizer)

include_directories("C:/Program Files (x86)/Eigen3/include")

add_executable(main main.cpp model.h model.cpp Rasterizer.h Rasterizer.cpp Shader.h Shader.cpp
               Texture.h tgaimage.cpp tgaimage.h Triangle.h Triangle.cpp)