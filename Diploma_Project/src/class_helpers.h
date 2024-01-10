#pragma once
#include <imgui.h>
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
    GLFWwindow* window;

    unsigned int shader = 0;
    unsigned int framebuffer = 0;
    unsigned int heightmap;

    int brush_type = 0;
    int mouse_mode = 0;
    
    float zoom = 1.0f;
    double last_x = 0.0f;
    double last_y = 0.0f;
    double curs_x = 0.0f;
    double curs_y = 0.0f;
    bool mouse_pressed = false;
    bool panel_hovered = false;
    int batman_panel_width = 100;
    int robin_panel_width = 100;
    float brush_size = 10;
    

    glm::vec3 transform = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::mat4 model = glm::mat4(1.0f);//for future add this to the quad class
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::ortho(-1.0f, +1.0f, -1.0f, +1.0f, 0.1f, 100.0f);

    int sel_tool = 0;//0 - move tool, 1 - terrain brush

    StateHandler();
    void attachFramebuffer(unsigned int framebuffer_);
    void attachShader(unsigned int shader_);
    void updMat(glm::mat4 matrix, const char* matrix_name);
    void updFloat(float num, const char* name);
    void updVec(glm::vec3 vec, const char* vec_name);
    void updVec(glm::vec2 vec, const char* vec_name);
    
};

class Quad
{
    //data to draw a quad
    float vertices[20] = {};
    int position_stride = 0;
    int uv_stride = 0 ;
    unsigned int indices[6] = {};
    const char* texture_path = "";
    

    VertexBuffer vb;
    IndexBuffer ib;
    unsigned int vao = 0;

    //functions
public:
    glm::mat4 model = glm::mat4(1.0f);
    unsigned int texture = 0;
    

    Quad();
    Quad(float vertices_[20], int pos_stride_, int uv_stride_, unsigned int indices_[6], const char* texture_path_);
    Quad(int width, int height);
    void initialize(bool use_texture);
    void changeSize(float canvas_ratio, bool w_ratio);
    void debug();
    void draw();


};
//FOR FUTURE
//class Canvas: public Quad
//{
//public:
//    int width = 480;
//    int height = 640;
//
//    Canvas(int width, int height);
//    void setSize(int height_, int width_);
//};
class UiHandler
{
    int i = 0;
    bool leftPanelOpen = true;
    float sliderPosX = 0;
    float sliderPosY = 0;
    float sliderWidth = 100;
public:
    void renderUI(StateHandler& state, Quad& canvas, int& w_width, int& w_height, int& canvas_width, int& canvas_height, float& resolution);
    void setCustomStyle();
    void setCustomFont(const char regular[], const char bold[]);
    void terrainPanel(StateHandler& state, int& w_width, int& w_height, int& canvas_width, int& canvas_height, float& resolution);
};