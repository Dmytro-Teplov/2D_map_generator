#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "class_helpers.h"

//#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>
//#include <gtc/type_ptr.hpp>
//#include "GL_helpers.h"


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
Quad::~Quad()
{

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
    -1.0f,   1.0f, 0.0, 0.0, 1.0,
     1.0f,   1.0f, 0.0, 1.0, 1.0,
     1.0f,  -1.0f, 0.0, 1.0, 0.0,
    -1.0f,  -1.0f, 0.0, 0.0, 0.0
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
void Quad::create(int width, int height)
{
    float vertices_[20] = {
    -1.0f,   1.0f, 0.0, 0.0, 1.0,
     1.0f,   1.0f, 0.0, 1.0, 1.0,
     1.0f,  -1.0f, 0.0, 1.0, 0.0,
    -1.0f,  -1.0f, 0.0, 0.0, 0.0
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

    ib.initialize(indices, std::size(indices));

    std::cout << glGetError();
    
    std::cout << glGetError();
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * position_stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * uv_stride, (void*)(sizeof(float) * position_stride - sizeof(float) * 2));//2 is the amount of uv coordinates

    


    // Load the image data
    if (use_texture) 
    {
        stbi_set_flip_vertically_on_load(true);
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
        //Unbind the texture object
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
void Quad::setTexture(const char* texture_path_)
{
    texture_path = texture_path_;
}
void Quad::setShader(unsigned int shader_)
{
    shader = shader_;
}
void Quad::calculateSSBB(StateHandler& state)
{
    glm::mat4 mvp = state.projection * state.view * model;

    glm::vec4 ssVertex_1 = mvp * glm::vec4(vertices[0], vertices[1], vertices[2],1.0);
    glm::vec4 ssVertex_2 = mvp * glm::vec4(vertices[10], vertices[11], vertices[12],1.0);
    
    glm::vec2 ssVertex_1xy = glm::vec2(ssVertex_1[0], ssVertex_1[1]);
    glm::vec2 ssVertex_2xy = glm::vec2(ssVertex_2[0], ssVertex_2[1]);

    glm::vec2 screenSpacePos_1 = ((ssVertex_1xy + glm::vec2(1.0f)) / 2.0f) * glm::vec2(state.w_width, state.w_height);
    glm::vec2 screenSpacePos_2 = ((ssVertex_2xy + glm::vec2(1.0f)) / 2.0f) * glm::vec2(state.w_width, state.w_height);
    /*std::cout << glm::to_string(screenSpacePos_1) << std::endl;
    std::cout << glm::to_string(screenSpacePos_2) << std::endl;*/
    screenSpacePos_1[0] = int(screenSpacePos_1[0]);
    screenSpacePos_1[1] = int(state.w_height - screenSpacePos_1[1]);
    screenSpacePos_2[0] = int(screenSpacePos_2[0]);
    screenSpacePos_2[1] = int(state.w_height - screenSpacePos_2[1]);
    ssbb = glm::mat2(screenSpacePos_1, screenSpacePos_2);
    //std::cout << glm::to_string(ssbb) << std::endl;
}
bool Quad::isInside(float posx, float posy)
{

    /*std::cout << posx << " " << posy << std::endl;
    std::cout << ssbb[0][0]<<"," << ssbb[0][1] << " " << ssbb[1][0] << "," << ssbb[1][1] << std::endl;
    std::cout << (posx > ssbb[0][0] && posx < ssbb[1][0] && posy> ssbb[0][1] && posy < ssbb[1][1]) << std::endl;
    std::cout << "-----------" << std::endl;*/

    return posx > ssbb[0][0] && posx < ssbb[1][0] && posy> ssbb[0][1] && posy < ssbb[1][1];

}
Quad Quad::operator=(const Quad& q)
{
    for (int i = 0; i < 20; i++) {
        this->vertices[i] = q.vertices[i];
        if (i < 6)
        {
            this->indices[i] = q.indices[i];
        }
    }
    this->position_stride = q.position_stride;
    this->uv_stride = q.uv_stride;

    this->texture_path = q.texture_path;
    this->shader = q.shader;

    this->x_dim = q.x_dim;
    this->y_dim = q.y_dim;

    this->vb = q.vb;
    this->ib = q.ib;
    this->vao = q.vao;
    this->ssbb = q.ssbb;

    this->model = q.model;
    this->texture = q.texture;
    return Quad();
}
void Quad::debug()
{
    std::cout << vertices[0] << " " << vertices[7] << std::endl;
    std::cout << indices[0] << " " << indices[5] << std::endl;
    std::cout << texture_path;
}
void Quad::draw()
{
    glBindVertexArray(vao);
    vb.bind();
    ib.bind();
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
void Quad::draw_instance()
{
    glBindVertexArray(vao);
    vb.bind();
    ib.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


}
void Quad::getReadyForDraw()
{
    vb.bind();
    glBindVertexArray(vao);
    ib.bind();
}

StateHandler::StateHandler()
{
}

void StateHandler::saveFbID(unsigned int framebuffer_)
{
    framebuffer = framebuffer_;
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
void StateHandler::updInt(int val, const char* name)
{
    int location = glGetUniformLocation(shader, name);
    glUniform1i(location, val);
}
void StateHandler::updSampler(int sampler, const char* name)
{
    int location = glGetUniformLocation(shader, name);
    glUniform1i(location, sampler);
}
void StateHandler::updVec(glm::vec2 vec, const char* vec_name)
{
    int location = glGetUniformLocation(shader, vec_name);
    glUniform2fv(location, 1, glm::value_ptr(vec));
}
void StateHandler::updVec(glm::vec3 vec, const char* vec_name)
{
    int location = glGetUniformLocation(shader, vec_name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}
void StateHandler::updVec(glm::vec4 vec, const char* vec_name)
{
    int location = glGetUniformLocation(shader, vec_name);
    glUniform4fv(location, 1, glm::value_ptr(vec));
}

void StateHandler::exportAsPNG(unsigned int textureID, int width, int height,int c_width, int c_height,const char* filename)
{
    unsigned char* imageData_raw = (unsigned char*)malloc(width * height * 4);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData_raw);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filename, width, height, 4, imageData_raw, width * 4);
}

void UiHandler::renderUI(StateHandler& state,Canvas& canvas, AssetHandler& assets, int w_width, int w_height, int canvas_width, int canvas_height, float resolution)
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
    middleLabel("Maps Generator");
    if (ImGui::Button("Move Tool",ImVec2(windowSize.x, 30)))//bad constants meh
    {
        state.sel_tool = 0;
    }
    middleLabel("Brushes");

    if (ImGui::Button("Terrain", ImVec2(windowSize.x, 30)))
    {
        state.sel_tool = 1;//MAKE SECOND TERRAIN TYPE
    }
    //if (ImGui::Button("Terrain 2", ImVec2(windowSize.x, 30)))
    //{
    //    state.sel_tool = 4;
    //}
    if (ImGui::Button("Water", ImVec2(windowSize.x, 30)))
    {
        state.sel_tool = 2;
    }
    if (ImGui::Button("Buildings", ImVec2(windowSize.x, 30)))
    {
        state.sel_tool = 3;
    }
    if (ImGui::Button("Flora", ImVec2(windowSize.x, 30)))
    {
        state.sel_tool = 4;
    }
    /*if (ImGui::Button("Paths", ImVec2(windowSize.x, 30)))
    {
        state.sel_tool = 5;
    }*/
    middleLabel("Canvas");
    if (ImGui::Button("Reset", ImVec2(windowSize.x, 30)))
    {
        
        state.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
        state.zoom = 1.0;
        state.reset = true;
        state.updMat(state.view, "view");
        state.updMat(state.projection, "projection");

    }
    
    middleLabel("Settings");
    ImGui::InputInt("Noise Complexity", &canvas.noise_compl);
    ImGui::InputFloat("Noise 1 Scale", &canvas.noise_1_scale);
    ImGui::InputFloat("Noise 2 Scale", &canvas.noise_2_scale);
    middleLabel("Export");
    if (ImGui::InputText("Name(with extension!)", state.export_name_data, IM_ARRAYSIZE(state.export_name_data)))
    {
    }
    if (ImGui::Button("Save into PNG", ImVec2(windowSize.x, 30)))
    {
        state.save = true;
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
            terrainPanel(state,canvas, w_width, w_height, canvas_width, canvas_height);
            break;
        case 2:
            waterPanel(state,canvas, w_width, w_height, canvas_width, canvas_height);
            break;
        case 3:
            buildingsPanel(state, canvas, assets, w_width, w_height, canvas_width, canvas_height);
            break;
        case 4:
            floraPanel(state, canvas, assets, w_width, w_height, canvas_width, canvas_height);
            break;
    }
    ImGui::Render();
    
}

void UiHandler::renderStartupUI(StateHandler& state, Canvas& canvas, int w_width, int w_height, int canvas_width, int canvas_height, unsigned int shader_)
{
    unsigned int curr_shader = state.shader;
    bool open_start = false;
    ImGui::Begin("Startup", &open_start, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::SetWindowSize(ImVec2(w_width, w_height));
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGuiStyle& style = ImGui::GetStyle();
    middleLabel("Canvas");
    ImGui::InputInt("canvas width", &canvas_width);
    ImGui::InputInt("canvas height", &canvas_height);
    if (ImGui::Button("Change the size"))
    {
        state.initial_start = false;
        //canvas.createTexture(canvas_width, canvas_height);
        state.attachShader(shader_);
        canvas.setSize(state, canvas_width, canvas_height);
        float res = (float)canvas_width / canvas_height;
        state.updFloat(res, "u_BgRes");
        state.attachShader(curr_shader);
    }
    ImGui::End();
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

void UiHandler::middleLabel(const char* text)
{
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPos(ImVec2((windowSize.x - ImGui::CalcTextSize(text).x) * 0.5, ImGui::GetCursorPos().y));
    ImGui::Text(text);
    ImGui::PopFont();
}

void UiHandler::terrainPanel(StateHandler& state, Canvas& canvas, int w_width, int w_height, int canvas_width, int canvas_height)
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

    middleLabel("Terrain Brush");
    ImGui::SliderFloat("Brush Size", &state.brush_size, 0.f, 250.f);// SHOULD BE PAINTER CLASS

    ImGui::SliderFloat("Brush Hardness", &state.brush_hardness, 0.f, 1.f);
    middleLabel("Terrain Settings");
    static float color[4] = { 0.84f,0.76f,0.67f,1.0f };
    static float color2[4] = { 0.84f,0.76f,0.67f,1.0f };
    static float color3[4] = { 0.84f,0.76f,0.67f,1.0f };
    ImGui::ColorEdit4("Color", color);
    static bool is_gradient = false;
    static bool outline = false;
    ImGui::Checkbox("Gradient", &is_gradient);
    ImGui::ColorEdit4("Second Color", color2);
    ImGui::Checkbox("Use step gradient", &canvas.use_step_gradient_t);
    ImGui::SliderInt("Steps", &canvas.steps_t, 1, 100);
    
    canvas.use_secondary_tc = is_gradient;
    canvas.terrain_c = glm::vec4(color[0], color[1], color[2], color[3]);
    canvas.terrain_secondary_c = glm::vec4(color2[0], color2[1], color2[2], color2[3]);
    ImGui::Checkbox("Outline", &outline);
    
    ImGui::ColorEdit4("Outline Color", color3);
    ImGui::SliderFloat("Outline Thickness", &canvas.outline_thickness, 0.f, 10.f);
    ImGui::SliderFloat("Outline Hardness", &canvas.outline_hardness, 0.f, 1.f);
    
    canvas.outline_c = glm::vec4(color3[0], color3[1], color3[2], color3[3]);
    canvas.use_outline = outline;
    
    ImGui::End();
}
void UiHandler::waterPanel(StateHandler& state, Canvas& canvas, int w_width, int w_height, int canvas_width, int canvas_height)
{
    ImGui::Begin("Water Tool", &leftPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
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
        if (!state.panel_hovered && state.sel_tool != 0)
            glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    middleLabel("Water Brush");
    ImGui::SliderFloat("Brush Size", &state.brush_size, 0.f, 250.f);

    ImGui::SliderFloat("Brush Hardness", &state.brush_hardness, 0.f, 1.f);
    middleLabel("Water Settings");
    static float color[4] = { 0.66f,0.76f,0.85f,1.0f };
    static float color2[4] = { 0.66f,0.76f,0.85f,1.0f };
    ImGui::ColorEdit4("Color", color);
    static bool is_gradient = false;
    ImGui::Checkbox("Gradient", &is_gradient);
    ImGui::ColorEdit4("Second Color", color2);
    ImGui::Checkbox("Use step gradient", &canvas.use_step_gradient_w);
    ImGui::SliderInt("Steps", &canvas.steps_w, 1, 100);
    
    canvas.water_c = glm::vec4(color[0], color[1], color[2], color[3]);
    canvas.use_secondary_wc = is_gradient;
    canvas.water_secondary_c = glm::vec4(color2[0], color2[1], color2[2], color2[3]);
    //std::cout << glm::to_string(canvas.water_c) << std::endl;
    //canvas.terrain_c2 = glm::vec4(color2[0], color2[1], color2[2], color2[3]);
    ImGui::End();
}
void UiHandler::buildingsPanel(StateHandler& state, Canvas& canvas, AssetHandler& assets, int w_width, int w_height, int canvas_width, int canvas_height)
{
    ImGui::Begin("Buildings Tool", &leftPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
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
        if ((!state.panel_hovered) && state.sel_tool != 0)
            glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    middleLabel("Buildings Brush");
    ImGui::SliderFloat("Brush Size", &state.brush_size, 0.f, 250.f);
    ImGui::Checkbox("Erase", &assets.erase_asset);
    middleLabel("Buildings Settings");
    ImGui::SliderFloat("Buildings Size", &assets.asset_size, 0.f, 15.f);
    ImGui::SliderFloat("Town density", &state.density_1, 0.01f, 1.0f);
    if (ImGui::Button("Adjust density of current asset", ImVec2(windowSize.x, 30)))
    {
        assets.genDistribution(canvas, 1.0 / state.density_1);
        assets.regenerate_mpds = true;
    }

    ImGui::End();
}
void UiHandler::floraPanel(StateHandler& state, Canvas& canvas, AssetHandler& assets, int w_width, int w_height, int canvas_width, int canvas_height)
{
    ImGui::Begin("Flora Tool", &leftPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
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
        if ((!state.panel_hovered) && state.sel_tool != 0)
            glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    middleLabel("Flora Brush");
    ImGui::SliderFloat("Brush Size", &state.brush_size, 0.f, 250.f);
    ImGui::Checkbox("Erase", &assets.erase_asset);
    middleLabel("Flora Settings");
    ImGui::SliderFloat("Flora Size", &assets.asset_size, 0.f, 15.f);
    ImGui::SliderFloat("Town density", &state.density_2, 0.01f, 1.0f);
    if (ImGui::Button("Adjust density of current asset", ImVec2(windowSize.x, 30)))
    {
        assets.genDistribution(canvas, 1.0 / state.density_2);
        assets.regenerate_mpds = true;
    }

    ImGui::End();
}

Canvas::Canvas(int width_, int height_)
{
    width = width_;
    height = height_;
    float vertices_[20] = {
    -1.0f,   1.0f, 0.0, 0.0, 1.0,
     1.0f,   1.0f, 0.0, 1.0, 1.0,
     1.0f,  -1.0f, 0.0, 1.0, 0.0,
    -1.0f,  -1.0f, 0.0, 0.0, 0.0
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
    
    fb_width = width_;
    fb_height = height_;
}

void Canvas::addFrameBufferQuad(int width_, int height_, unsigned int shader_, const char* texture_path_)
{
    Quad frm_bffr(width_, height_);
    frm_bffr.setTexture(texture_path_);
    frm_bffr.initialize(false);
    frm_bffr.debug();
    frm_bffr.setShader(shader_);
    
    // Load the image data
    
    int channels;
    int width, height;
    unsigned char* imageData = stbi_load(texture_path_, &width, &height, &channels, 4);

    if (imageData == nullptr) {
        std::cout << stbi_failure_reason();
        stbi_image_free(imageData);
    }

    canvas_rgba = imageData;

    //unsigned int texture;
    glGenTextures(1, &frm_bffr.texture);
    glActiveTexture(GL_TEXTURE0);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, frm_bffr.texture);

    // Upload the image data to the texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    

    // Specify the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    frm_buffr = frm_bffr;
}

void Canvas::setSize(StateHandler& state, int width_, int height_)
{
    width = width_;
    height = height_;
    state.canvas_height = height_;
    state.canvas_width = width_;
    model = glm::scale(glm::mat4(1.0f), glm::vec3(width_, height_, 1.0f));



    unsigned char* imageData = (unsigned char*)malloc(width * height * 4);
    unsigned char* maskData = (unsigned char*)malloc(width * height * 4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 4;
            imageData[index + 0] = 0; // Red
            imageData[index + 1] = 0;   // Green
            imageData[index + 2] = 255;   // Blue
            imageData[index + 3] = 255;   // Alpha

            maskData[index + 0] = 0; // Red
            maskData[index + 1] = 0;   // Green
            maskData[index + 2] = 0;   // Blue
            maskData[index + 3] = 255;   // Alpha
        }
    }

    // get the texture data
    /*glBindTexture(GL_TEXTURE_2D, fb_texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);*/

    canvas_rgba = imageData;
    buildings_rgba = maskData;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas_rgba);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (state.shader != shader) 
    {
        unsigned int curr_shader = state.shader;
        state.attachShader(shader);
        state.updMat(model, "model");
        state.attachShader(curr_shader);
        std::cout << "equal";
    }
    else
    {
        std::cout << "else";
        state.updMat(model, "model");
    }
    
    
}

void Canvas::setTexture(const char* texture_path_)
{
    int channels;
    int width, height;
    unsigned char* imageData = stbi_load(texture_path_, &width, &height, &channels, 4);

    if (imageData == nullptr) {
        std::cout << stbi_failure_reason();
        stbi_image_free(imageData);
    }
    fb_width = width;
    fb_height = height;
    canvas_rgba = imageData;

    unsigned char* maskData = (unsigned char*)malloc(width * height * channels);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * channels;
            maskData[index + 0] = 0; // Red
            maskData[index + 1] = 0;   // Green
            maskData[index + 2] = 0;   // Blue
            maskData[index + 3] = 255;   // Alpha
        }
    }

    buildings_rgba = maskData;
    //unsigned int texture;
    glGenTextures(1, &fb_texture);
    glActiveTexture(GL_TEXTURE0);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, fb_texture);

    // Upload the image data to the texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);


    // Specify the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    //texture_path = texture_path_;
}

void Canvas::createTexture(int width,int height)
{
    int channels=4;
    //int width, height;
    unsigned char* imageData = (unsigned char*)malloc(width * height * channels);
    unsigned char* maskData = (unsigned char*)malloc(width * height * channels);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * channels;
            imageData[index + 0] = 0; // Red
            imageData[index + 1] = 0;   // Green
            imageData[index + 2] = 255;   // Blue
            imageData[index + 3] = 255;   // Alpha

            maskData[index + 0] = 0; // Red
            maskData[index + 1] = 0;   // Green
            maskData[index + 2] = 0;   // Blue
            maskData[index + 3] = 255;   // Alpha
        }
    }
    fb_width = width;
    fb_height = height;
    canvas_rgba = imageData;
    buildings_rgba = maskData;
    //unsigned int texture;
    glGenTextures(1, &fb_texture);
    glActiveTexture(GL_TEXTURE0);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, fb_texture);

    // Upload the image data to the texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);


    // Specify the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    //texture_path = texture_path_;
}

Painter::Painter()
{


}

void Painter::paint(float posx, float posy, Canvas& canvas, StateHandler& state)
{
    //std::cout << posx - canvas.ssbb[0][0] << "," << posy - canvas.ssbb[0][1] << std::endl;
    if (canvas.isInside(posx,posy))
    {
        int abs_posx = (posx - canvas.ssbb[0][0])/state.zoom;
        int abs_posy = (canvas.fb_height - (posy - canvas.ssbb[0][1]) / state.zoom);
        int index = (canvas.fb_width * abs_posy + abs_posx) * 4;
        //std::cout << canvas.fb_width << " " << canvas.fb_height << std::endl;
        brush_size = brush_size / state.zoom;
        //std::cout << canvas.ssbb[0][0] << " " << canvas.ssbb[0][1] << std::endl;

        paintCanvas(canvas.canvas_rgba, abs_posx, abs_posy, canvas.fb_width, canvas.fb_height, state.sel_tool);

        // Bind the texture object
        glBindTexture(GL_TEXTURE_2D, canvas.fb_texture);

        // Upload the image data to the texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas.fb_width, canvas.fb_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas.canvas_rgba);

    }
}
void Painter::paint(float posx, float posy, Canvas& canvas, StateHandler& state, AssetHandler& assets)
{
    //std::cout << posx - canvas.ssbb[0][0] << "," << posy - canvas.ssbb[0][1] << std::endl;
    if (canvas.isInside(posx, posy))
    {
        int abs_posx = (posx - canvas.ssbb[0][0]) / state.zoom;
        int abs_posy = (canvas.fb_height - (posy - canvas.ssbb[0][1]) / state.zoom);
        int index = (canvas.fb_width * abs_posy + abs_posx) * 4;
        //std::cout << canvas.fb_width << " " << canvas.fb_height << std::endl;
        brush_size = brush_size / state.zoom;
        //std::cout << canvas.ssbb[0][0] << " " << canvas.ssbb[0][1] << std::endl;
        paintAssets(canvas.buildings_rgba, assets.asset_type, abs_posx, abs_posy, canvas.fb_width, canvas.fb_height, assets.erase_asset);

        // Bind the texture object
        glBindTexture(GL_TEXTURE_2D, canvas.fb_texture);

        // Upload the image data to the texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas.fb_width, canvas.fb_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas.canvas_rgba);

    }
}

void Painter::paintCanvas(unsigned char*& canvas_rgba,int abs_posx, int abs_posy, int width, int height, int mode)
{
    //std::cout << sizeof(canvas_rgba) / sizeof(canvas_rgba[0]) << std::endl;
    for (int i = -brush_size / 2; i < brush_size / 2; i++)
    {
        for (int j = -brush_size / 2; j < brush_size / 2; j++)
        {
            int index = (width * (abs_posy + j) + abs_posx + i) * 4;
            if ((std::pow(i * i + j * j, 0.5) * 2 < brush_size) && (index + 2 < width * height * 4) && (index > 0))
            {
                float hardness = (1 - ((float)std::pow(i * i + j * j, 0.5) * 2) / brush_size) * brush_hardness;
                //std::cout << hardness;

                switch (mode)
                {
                case 1:
                    canvas_rgba[index] = 255 * hardness + canvas_rgba[index] * (1 - hardness);
                    canvas_rgba[index + 1] = 0;
                    canvas_rgba[index + 2] = 255 - canvas_rgba[index];
                    break;
                case 2:
                    canvas_rgba[index + 2] = 255 * hardness + canvas_rgba[index + 2] * (1 - hardness);
                    canvas_rgba[index + 1] = 0;
                    canvas_rgba[index] = 255 - canvas_rgba[index + 2];
                    break;
                
                case 5:
                    canvas_rgba[index + 1] = 255 * hardness + canvas_rgba[index + 1] * (1 - hardness);
                    break;
                }
          
            }
        }
    }
}
void Painter::paintAssets(unsigned char*& canvas_rgba, unsigned int type, int abs_posx, int abs_posy, int width, int height, bool erase)
{
    //std::cout << sizeof(canvas_rgba) / sizeof(canvas_rgba[0]) << std::endl;
    for (int i = -brush_size / 2; i < brush_size / 2; i++)
    {
        for (int j = -brush_size / 2; j < brush_size / 2; j++)
        {
            int index = (width * (abs_posy + j) + abs_posx + i) * 4;
            if ((std::pow(i * i + j * j, 0.5) * 2 < brush_size) && (index + 2 < width * height * 4) && (index > 0))
            {
                float hardness = (1 - (std::pow(i * i + j * j, 0.5) * 2) / brush_size) * brush_hardness;
                canvas_rgba[index+type] = 255*(!erase);
            }
        }
    }
}

Painter Painter::operator=(const Painter& p)
{
    this->brush_hardness = p.brush_hardness;
    this->brush_size = p.brush_size;
    this->explicit_height = p.explicit_height;
    return Painter();

}
AssetHandler::AssetHandler(bool custom,const char* texture_path) : asset(50, 50) {
    if (custom)
    {
        asset.texture_path = texture_path;
        int channels;
        int width, height;
        unsigned char* imageData = stbi_load(texture_path, &width, &height, &channels, 4);
        if (imageData == nullptr) {
            std::cout << stbi_failure_reason();
            stbi_image_free(imageData);
            asset.texture_path = "res/assets/default_assets.png";
        }
        else
        {
            stbi_image_free(imageData);
        }
    }
    else 
    {
        asset.texture_path = "res/assets/default_assets.png";
    }
    asset.initialize(true);
    asset.debug();
}

struct less_than_key
{
    inline bool operator() (const glm::vec3& struct1, const glm::vec3& struct2)
    {
        return (struct1[1] > struct2[1]);
    }
};

void AssetHandler::genDistribution(Canvas& canvas, float radius)
{
    std::cout << "\n" << canvas.ssbb[0][0] << "\t" << canvas.ssbb[0][1] << std::endl;
    std::cout << "\n" << canvas.ssbb[1][0] << "\t" << canvas.ssbb[1][1] << std::endl;
    /*auto kXMin = std::array<float, 2>{{canvas.ssbb[0][0], canvas.ssbb[0][1]}};
    auto kXMax = std::array<float, 2>{{canvas.ssbb[1][0], canvas.ssbb[1][1]}};*/
    auto kXMin = std::array<float, 2>{{-(float)canvas.width, -(float)canvas.height}};
    auto kXMax = std::array<float, 2>{{(float)canvas.width, (float)canvas.height}};
    std::vector<std::array<float, 2>> pos = thinks::PoissonDiskSampling(radius, kXMin, kXMax);
    number_of_points = pos.size();
    mpds_positions.clear();
    asset_IDs.clear();
    mpds_positions.reserve(number_of_points);
    asset_IDs.reserve(number_of_points);

    //int index = 0;
    //int posx, posy;
    for (size_t i = 0; i < number_of_points; ++i)
    {
        mpds_positions.push_back(glm::vec3(pos[i][0], pos[i][1], i));
        asset_IDs.push_back(i);
    }
    number_of_points = mpds_positions.size();
    if (number_of_points)
    {
        std::sort(mpds_positions.begin(), mpds_positions.end(), less_than_key());

        // Setting up vertex attributes for instance positions
        glBindVertexArray(asset.vao);
        glGenBuffers(1, &instancePos_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, instancePos_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * number_of_points, &mpds_positions[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, instancePos_VBO);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1);

        glGenBuffers(1, &instanceID_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceID_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(int) * number_of_points, &asset_IDs[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, instanceID_VBO);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(3, 1);

        glBindVertexArray(0);
        
    }

}

void AssetHandler::draw(StateHandler& state, Canvas& canvas, unsigned int shader_, glm::mat4 cust_view, int isFB)
{
    if (number_of_points && regenerate_mpds)
    {
        int index = 0;
        int posx, posy;
        asset_positions.clear();
        asset_IDs.clear();
        asset_positions.reserve(number_of_points);
        asset_IDs.reserve(number_of_points);
        for (size_t i = 0; i < number_of_points; ++i)
        {
            //(y * width + x)* channels
            posx = (mpds_positions[i][0] + canvas.width) * 0.5;
            posy = (canvas.height + mpds_positions[i][1]) * 0.5;
            index = (canvas.width * posy + posx) * 4;
            //std::cout << (int)canvas.buildings_rgba[index] << std::endl;
            if ((int)canvas.buildings_rgba[index+asset_type] != 0)
            {
                asset_positions.push_back(mpds_positions[i]);
                asset_IDs.push_back(mpds_positions[i][2]);
            }

        }
        number_of_assets = asset_positions.size();
        //state.regenerate_buildings = false; //PROBLEM
    }
    if (number_of_assets) 
    {
        asset.setShader(shader_);
        unsigned int curr_shader = state.shader;
        state.attachShader(shader_);
        state.updMat(state.projection, "projection");
        state.updMat(state.view_relative, "view");
        state.updMat(asset.model, "model");
        state.updMat(cust_view, "u_asset_view");
        state.updFloat(10 * asset_size, "u_asset_scale");
        state.updVec(canvas.water_c, "u_water_color");
        state.updSampler(0, "asset_texture");
        state.updSampler(1, "background");
        state.updInt(isFB, "u_isFB");
        state.updInt(asset_type, "u_asset_type");

        glBindVertexArray(asset.vao);

        glBindBuffer(GL_ARRAY_BUFFER, instancePos_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * number_of_assets, &asset_positions[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, instanceID_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(int) * number_of_assets, &asset_IDs[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glActiveTexture(GL_TEXTURE0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, asset.texture);
        glActiveTexture(GL_TEXTURE1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, bgTexture_ID);

        //glBindTexture(GL_TEXTURE_2D,asset.texture);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, number_of_assets);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        state.attachShader(curr_shader);
    }
    
    
}
