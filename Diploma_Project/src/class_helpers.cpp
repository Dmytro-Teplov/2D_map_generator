#define STB_IMAGE_IMPLEMENTATION

#include "class_helpers.h"
#include <stb_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "GL_helpers.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}
static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << error << " " << function << " " << file << " " << line << std::endl;
        return false;
    }
    return true;
}

Quad::Quad()
{
    for (int i = 0; i < 20; i++) {
        vertices[i] = 0.f;
    }
}
Quad::Quad(float vertices_[20], int pos_stride_, int uv_stride_, unsigned int indices_[6], const char* texture_path_)
{
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
Quad::Quad(int width, int height)
{
    float vertices_[20] = {
    -1.0f,  -1.0f, 0.0, 0.0, 1.0,
     1.0f,  -1.0f, 0.0, 1.0, 1.0,
     1.0f,   1.0f, 0.0, 1.0, 0.0,
    -1.0f,   1.0f, 0.0, 0.0, 0.0
    };
    unsigned int indices_[6] = {
    0,1,2,
    2,3,0
    };
    int pos_stride_ = 5;
    int uv_stride_ = 5;
    for (int i = 0; i < 20; i++) {
        vertices[i] = vertices_[i];
        if (i < 6)
        {
            indices[i] = indices_[i];
        }
    }
    position_stride = pos_stride_;
    uv_stride = uv_stride_;
    texture_path = "";
}
void Quad::initialize(bool use_texture)
{
     
    //unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
   
    vb.initialize(vertices, std::size(vertices) * sizeof(float));

    std::cout << glGetError();
    
    std::cout << glGetError();
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * position_stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * uv_stride, (void*)(sizeof(float) * position_stride - sizeof(float) * 2));//2 is the amount of uv coordinates

    ib.initialize(indices, std::size(indices));


    // Load the image data
    if (use_texture) 
    {
        int channels;
        int width, height;
        unsigned char* imageData = stbi_load(texture_path, &width, &height, &channels, 4);
        if (imageData == nullptr) {
            std::cout << stbi_failure_reason();
            stbi_image_free(imageData);
        }
        //unsigned int texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);

        // Bind the texture object
        glBindTexture(GL_TEXTURE_2D, texture);

        // Upload the image data to the texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        std::cout << width << " " << height << " " << channels << " " << glGetError();

        // Specify the texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        std::cout << glGetError();
        // Unbind the texture object
        //glBindTexture(GL_TEXTURE_2D, 0);

        //glUseProgram(shader);
        //std::cout << glGetError();
    }
    
}
void Quad::changeSize(float canvas_ratio, bool w_ratio)
{
    int height = 1, width = 1;
    if (w_ratio)
    {
        height = canvas_ratio;
        width = 1.0f;
    }
    else
    {
        width = width / height;
        height = 1.0f;
    }
    for (int i = 0; i < std::size(vertices); i += position_stride)
    {
        vertices[i] = ((vertices[i] > 0) - (vertices[i] < 0)) * width;
        vertices[i + 1] = ((vertices[i + 1] > 0) - (vertices[i + 1] < 0)) * height;
        std::cout << vertices[i] << " " << vertices[i + 1] << std::endl;
    }
}
void Quad::debug()
{
    std::cout << vertices[0] << " " << vertices[7] << std::endl;
    std::cout << indices[0] << " " << indices[5] << std::endl;
    std::cout << texture_path;
}
void Quad::draw()
{
    vb.bind();
    glBindVertexArray(vao);
    ib.bind();
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

StateHandler::StateHandler()
{
}

void StateHandler::attachFramebuffer(unsigned int framebuffer_)
{
    framebuffer = framebuffer_;
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
}
void StateHandler::attachShader(unsigned int shader_)
{
    shader = shader_;
    glUseProgram(shader);
}
void StateHandler::updMat(glm::mat4 matrix, const char* matrix_name)
{
    int location = glGetUniformLocation(shader, matrix_name);

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void StateHandler::updFloat(float num, const char* name)
{
    int location = glGetUniformLocation(shader, name);
    glUniform1f(location, num);
}

void StateHandler::updVec(glm::vec3 vec, const char* vec_name)
{
    int location = glGetUniformLocation(shader, vec_name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}
void StateHandler::updVec(glm::vec2 vec, const char* vec_name)
{
    int location = glGetUniformLocation(shader, vec_name);
    glUniform2fv(location, 1, glm::value_ptr(vec));
}

void UiHandler::renderUI(StateHandler& state,Quad& canvas, int& w_width, int& w_height, int& canvas_width, int& canvas_height, float& resolution)
{
    ImGui::Begin("Quests", &leftPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::SetWindowSize(ImVec2(windowSize.x, w_height));
    state.batman_panel_width = windowSize.x;
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    
    if (ImGui::IsWindowHovered())
    {
        glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        state.panel_hovered = true;
    }
    else
    {
        if(state.sel_tool != 0)
            glfwSetInputMode(state.window, GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
    }


    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    sliderPosX = (windowSize.x - ImGui::CalcTextSize("Maps Generator").x) * 0.5;
    ImGui::SetCursorPos(ImVec2(sliderPosX, 0));
    ImGui::Text("Maps Generator");
    ImGui::PopFont();
   
    if (ImGui::Button("Move Tool",ImVec2(windowSize.x, 30)))//bad constants meh
    {
        state.sel_tool = 0;
    }
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPos(ImVec2((contentRegionAvail.x - ImGui::CalcTextSize("Brushes").x) * 0.5, ImGui::GetCursorPos().y));
    ImGui::Text("Brushes");
    ImGui::PopFont();
    if (ImGui::Button("Terrain", ImVec2(windowSize.x, 30)))
    {
        state.sel_tool = 1;
    }
    if (ImGui::Button("Water", ImVec2(windowSize.x, 30)))
    {
    }
    if (ImGui::Button("Buildings", ImVec2(windowSize.x, 30)))
    {
    }
    if (ImGui::Button("Flora", ImVec2(windowSize.x, 30)))
    {
    }
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPos(ImVec2((windowSize.x - ImGui::CalcTextSize("Canvas").x) * 0.5, ImGui::GetCursorPos().y));
    ImGui::Text("Canvas");
    ImGui::PopFont();
    ImGui::InputInt("canvas width", &canvas_width);
    ImGui::InputInt("canvas height", &canvas_height);
    if (ImGui::Button("Change the size"))
    {
        std::cout << w_width << " " << w_height << std::endl;
        std::cout << canvas_width << " " << canvas_height << std::endl;
        canvas.model = glm::scale(glm::mat4(1.0f), glm::vec3(canvas_width, canvas_height, 1.0f));
        state.updMat(canvas.model, "model");
        //resolution = (float)canvas_width / canvas_height;
        //state.updFloat(resolution, "u_BgRes");

    }
    if (ImGui::Button("Reset"))
    {
        
        state.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
        state.updMat(state.view, "view");
        state.updMat(state.projection, "projection");

    }
    sliderPosX = (windowSize.x - ImGui::CalcTextSize("Zoom").x) * 0.5;
    sliderPosY = windowSize.y - ImGui::GetTextLineHeightWithSpacing() * 2;
    
    ImGui::SetCursorPos(ImVec2(sliderPosX, sliderPosY));
    ImGui::Text("Zoom");

    // Calculate the position for the centered slider
    sliderWidth = (3.f/4.f)* windowSize.x; // Adjust the width as needed
    sliderPosX = (windowSize.x - sliderWidth)*0.5;
    sliderPosY = windowSize.y - ImGui::GetTextLineHeightWithSpacing();
    ImGui::SetCursorPos(ImVec2(sliderPosX, sliderPosY));
    ImGui::SliderFloat("##", &state.zoom, 0.f, 25.f);
    ImGui::End();
    
    switch (state.sel_tool) 
    {
        case 0:
            state.robin_panel_width = w_width;
            break;
        case 1:
            terrainPanel(state, w_width, w_height, canvas_width, canvas_height, resolution);
            break;
    }
    ImGui::Render();
    
}

void UiHandler::setCustomStyle()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 0;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;
}

void UiHandler::setCustomFont(const char regular[], const char bold[])
{
    ImGuiIO& io = ImGui::GetIO();

    // Load a custom font
    io.Fonts->AddFontFromFileTTF(regular, 20.0f);
    // Load a custom font
    io.Fonts->AddFontFromFileTTF(bold, 30.0f);
    

    // Set the font for the current session
    io.FontDefault = io.Fonts->Fonts.front();
}

void UiHandler::terrainPanel(StateHandler& state, int& w_width, int& w_height, int& canvas_width, int& canvas_height, float& resolution)
{
    ImGui::Begin("Terrain Tool", &leftPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::SetWindowSize(ImVec2(windowSize.x, w_height));
    ImGui::SetWindowPos(ImVec2(w_width - windowSize.x, 0));
    state.robin_panel_width = w_width - windowSize.x;
    if (ImGui::IsWindowHovered())
    {
        state.panel_hovered = true;
        glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        if(!state.panel_hovered && state.sel_tool != 0)
            glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    sliderPosX = (windowSize.x - ImGui::CalcTextSize("Brush Size").x) * 0.5;
    sliderPosY = windowSize.y - ImGui::GetTextLineHeightWithSpacing() * 2;

    ImGui::SetCursorPos(ImVec2(sliderPosX, 0));
    ImGui::Text("Brush Size");

    // Calculate the position for the centered slider
    sliderWidth = (3.f / 4.f) * windowSize.x; // Adjust the width as needed
    sliderPosX = (windowSize.x - sliderWidth) * 0.5;
    sliderPosY = windowSize.y - ImGui::GetTextLineHeightWithSpacing();
    ImVec2 curPos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(sliderPosX, curPos.y));
    ImGui::SliderFloat("##", &state.brush_size, 0.f, 250.f);


    ImGui::End();
}
