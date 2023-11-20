#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "GL_helpers.h"

class StateHandler
{

public:
    unsigned int shader = 0;

    int brush_type = 0;
    int mouse_mode = 0;
    
    float zoom = 1.0f;
    double last_x = 0.0f;
    double last_y = 0.0f;
    bool mouse_pressed = false;

    glm::vec3 transform = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 100.0f, 1.0f));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);

    StateHandler();
    void attachShader(unsigned int shader_);
    void updMat(glm::mat4 matrix, const char* matrix_name);
    void updFloat(float num, const char* name);
    
};

class Quad
{
    //data to draw a quad
    float vertices[20] = {};
    int position_stride = 0;
    int uv_stride = 0 ;
    unsigned int indices[6] = {};
    const char* texture_path = "";
    unsigned int texture;
    VertexBuffer vb;
    IndexBuffer ib;
    unsigned int vao;

    //functions
public:
    Quad();
    Quad(float vertices_[20], int pos_stride_, int uv_stride_, unsigned int indices_[6], const char* texture_path_);
    void initialize();
    void changeSize(float canvas_ratio, bool w_ratio);
    void debug();
    void draw();


};

