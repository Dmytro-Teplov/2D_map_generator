#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "GL_helpers.h"



class Quad
{
    //data to draw a quad
    unsigned int shader;
    float vertices[20];
    int position_stride;
    int uv_stride;
    unsigned int indices[6];
    const char* texture_path;
    VertexBuffer vb;
    IndexBuffer ib;

    //uniform locations, potentially not needed
    int color_loc;
    int text_loc;
    int size_loc;
    int proj_loc;
    //functions
public:
    Quad();
    Quad(unsigned int shader_, float vertices_[20], int pos_stride_, int uv_stride_, unsigned int indices_[6], const char* texture_path_);
    void initialize();
    void changeColor(float col);
    void changeSize(float canvas_ratio, bool w_ratio);
    void updMat(glm::mat4 matrix, const char* matrix_name);
    void updFloat(float num, const char* name);
    void debug();
    void draw();


};

