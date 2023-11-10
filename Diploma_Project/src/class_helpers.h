#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>




class Quad
{
    //data to draw a quad
    unsigned int shader;
    float vertices[20];
    int position_stride;
    int uv_stride;
    unsigned int indices[6];
    const char* texture_path;

    //uniform locations, potentially not needed
    int color_loc;
    int text_loc;
    int size_loc;
    int proj_loc;
    //functions
public:
    Quad()
    {
        shader = -1;
        for (int i = 0; i < 20; i++) {
            vertices[i] = 0.f;
        }
    }
    Quad(unsigned int shader_, float vertices_[20], int pos_stride_, int uv_stride_, unsigned int indices_[6], const char* texture_path_)
    {
        shader = shader_;
        for (int i = 0; i < 20; i++) {
            vertices[i] = vertices_[i];
            if (i < 6)
            {
                indices[i] = indices_[i];
            }
        }
        position_stride = pos_stride_;
        uv_stride = uv_stride_;
        texture_path = texture_path_;

    }
    void initialize();
    void changeColor(float col);
    void changeSize(float canvas_ratio, bool w_ratio);
    void updMat(glm::mat4 matrix, const char* matrix_name);
    void updFloat(float num, const char* name);
    void debug();
    void draw();


};

