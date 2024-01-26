
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "class_helpers.h"
#include <stb_image.h>

StateHandler state;
glm::vec3 sun = glm::vec3(1.0, 1.0, 1.0);
int w_width, w_height;
glm::mat4 relative_cursor = glm::mat4(1.0f);
Canvas canvas(100, 100);
void windowResizeHandler(int window_width, int window_height) {
    glViewport(0, 0, window_width, window_height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    state.zoom += (float)yoffset * state.zoom / 10.0f;
    if (state.zoom < 0.1f)
        state.zoom = 0.1f;
    if (state.zoom > 25.0f)
        state.zoom = 25.0f;
}
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (state.sel_tool == 0) 
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            //int w_width, w_height;
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            glfwGetWindowSize(window, &w_width, &w_height);

            if (state.last_x > state.batman_panel_width && state.last_x < state.robin_panel_width)
            {
                state.mouse_pressed = true;

            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            /*double x, y;
            glfwGetCursorPos(window, &x, &y);*/
            glfwGetWindowSize(window, &w_width, &w_height);
            if (state.curs_x > state.batman_panel_width && state.curs_x < state.robin_panel_width) {
                state.mouse_pressed = false;
                state.view = glm::translate(state.view, state.transform);
                canvas.calculateSSBB(state);
            }
        }
    }
    else if (state.sel_tool == 1|| state.sel_tool == 2)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            if (canvas.isInside(state.last_x, state.last_y))
            {
                state.brush_pressed = true;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            state.brush_pressed = false;
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        {
            //int w_width, w_height;
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            glfwGetWindowSize(window, &w_width, &w_height);

            if (state.last_x > state.batman_panel_width && state.last_x < state.robin_panel_width)
            {
                state.mouse_pressed = true;

            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        {
            /*double x, y;
            glfwGetCursorPos(window, &x, &y);*/
            glfwGetWindowSize(window, &w_width, &w_height);
            if (state.curs_x > state.batman_panel_width && state.curs_x < state.robin_panel_width) {
                state.mouse_pressed = false;
                state.view = glm::translate(state.view, state.transform);
                canvas.calculateSSBB(state);
            }
        }
    }

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    
    float canvas_aspect_ratio = 1.0;
    int canvas_width = 640, canvas_height = 480;
    window = glfwCreateWindow(1280, 720, "Magic Maps", NULL, NULL);
    glfwGetWindowSize(window, &w_width, &w_height);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        std::cout << "Error";
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);


    float positions[20] = {
    -1.0f,  -1.0f, 0.0, 0.0, 1.0,
     1.0f,  -1.0f, 0.0, 1.0, 1.0,
     1.0f,   1.0f, 0.0, 1.0, 0.0,
    -1.0f,   1.0f, 0.0, 0.0, 0.0
    };

    float positions_2[20] = {
     0.0f,  0.0f, 0.0, 0.0, 1.0,
     1.0f,  0.0f, 0.0, 1.0, 1.0,
     1.0f,  1.0f, 0.0, 1.0, 0.0,
     0.0f,  1.0f, 0.0, 0.0, 0.0
    };

    float positions_3[20] = {
     -2.0f,  -2.0f, 0.0, 0.0, 1.0,
     -1.0f,  -2.0f, 0.0, 1.0, 1.0,
     -1.0f,  -1.0f, 0.0, 1.0, 0.0,
     -2.0f,  -1.0f, 0.0, 0.0, 0.0
    };

    unsigned int indices[6] = {
    0,1,2,
    2,3,0
    };


    ShaderProgramSource heightmap_sources, terrain_sources, cursor_sources;

    heightmap_sources = ParseShader("res/shaders/mg_heightmap.shader");
    unsigned int heightmap_shader = CreateShader(heightmap_sources.vertexShader, heightmap_sources.fragmentShader);
    
    terrain_sources = ParseShader("res/shaders/mg_terrain.shader");
    unsigned int terrain_shader = CreateShader(terrain_sources.vertexShader, terrain_sources.fragmentShader);

    cursor_sources = ParseShader("res/shaders/mg_cursor.shader");
    unsigned int cursor_shader = CreateShader(cursor_sources.vertexShader, cursor_sources.fragmentShader);
    
    
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current path is " << currentPath << std::endl;

    //Quad assets[128];
    
    canvas.initialize(false);
    //canvas.addFrameBufferQuad(w_width, w_height, heightmap_shader, "res/assets/default_map.png");
    canvas.setTexture("res/assets/default_map.png");
    canvas.debug();
    canvas.setShader(terrain_shader);
    

    Quad frm_buffr(w_width, w_height);
    //frm_buffr.setTexture("res/assets/default_map.png");
    frm_buffr.initialize(false);
    frm_buffr.texture = canvas.fb_texture;
    frm_buffr.debug();
    frm_buffr.setShader(heightmap_shader);

    Quad brush(100,100);
    brush.initialize(false);
    brush.debug();
    brush.setShader(cursor_shader);

    state.window = window;

    state.attachShader(terrain_shader);
    canvas.setSize(state, canvas_width, canvas_height);
    //canvas.model = glm::scale(glm::mat4(1.0f), glm::vec3(canvas_width, canvas_height, 1.0f));
    state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
    //state.updMat(canvas.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");
    state.updVec(sun, "u_sun_pos");
    float res = (float)canvas_width / canvas_height;
    state.updFloat(res, "u_BgRes");


    state.attachShader(cursor_shader);
    //brush.model = glm::scale(glm::mat4(1.0f), glm::vec3(state.brush_size, state.brush_size, 1.0f));
    state.updMat(brush.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");
    state.updVec(glm::vec2(w_width, w_height), "u_resolution");
    state.updVec(glm::vec2(0, 0), "u_pos");
    state.updFloat(state.brush_size, "u_circle_size");

    state.attachShader(heightmap_shader);
    state.updMat(frm_buffr.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");
    state.updFloat(res, "u_BgRes");
    
    

    unsigned int heightmap;
    glGenTextures(1, &heightmap);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_width, w_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int fb;
    glGenFramebuffers(1,&fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        std::cout << "frame buffer is done\n";
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightmap, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    canvas.texture = heightmap;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float c = 1.0f;
    float sz = 100.0f;

    double x = 0;
    double y = 0;

    glm::mat4 view_relative;

    UiHandler ui;
    ui.setCustomFont("res/fonts/AtkinsonHyperlegible-Regular.ttf", "res/fonts/AtkinsonHyperlegible-Bold.ttf");
    ui.setCustomStyle();

    Painter painter;

    //HATE THIS
    glfwGetWindowSize(window, &w_width, &w_height);
    state.w_width = w_width;
    state.w_height = w_height;
    glViewport(0, 0, w_width, w_height);
    glfwGetCursorPos(window, &state.curs_x, &state.curs_y);
    canvas.calculateSSBB(state);

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwGetWindowSize(window, &w_width, &w_height);
        state.w_width = w_width;
        state.w_height = w_height;
        glViewport(0, 0, w_width, w_height);
        glfwGetCursorPos(window, &state.curs_x, &state.curs_y);

        //FRAME BUFFER GENERATION
        glBindTexture(GL_TEXTURE_2D, heightmap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_width, w_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        state.attachFramebuffer(fb);
        frm_buffr.draw();

        //CANVAS DRAWING 
        state.attachShader(terrain_shader);
        state.attachFramebuffer(0);
        state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
        state.updMat(state.projection, "projection");
        state.transform = glm::vec3(state.zoom);
        view_relative = glm::scale(state.view, glm::vec3(state.zoom, state.zoom, state.zoom));
        state.updMat(view_relative, "view");
        if (state.mouse_pressed)
        {
            //state.view = view_relative;
            glfwGetCursorPos(window, &x, &y);

            if (x > state.batman_panel_width)
            {
                state.transform = glm::vec3(2)*glm::vec3((x - state.last_x), -(y - state.last_y), 0.0f);
                view_relative = glm::translate(view_relative, state.transform / glm::vec3(state.zoom));
            }

        }
        
        state.updMat(view_relative, "view");
        canvas.draw();

        //CURSOR DRAWING
        if (state.sel_tool != 0)
        {

            state.attachShader(cursor_shader);
            relative_cursor = glm::translate(brush.model, glm::vec3(state.curs_x - w_width / 2.0, -state.curs_y + w_height / 2.0, 0) * glm::vec3(2));
            relative_cursor = glm::scale(relative_cursor, glm::vec3(state.brush_size));
            state.updMat(state.projection, "projection");
            state.updMat(relative_cursor, "model");
            state.updFloat(state.brush_size, "u_circle_size");
            state.updVec(glm::vec2(state.curs_x, w_height - state.curs_y), "u_pos");
            brush.draw();
            if (state.brush_pressed)
            {
                glfwGetCursorPos(window, &x, &y);
                painter.brush_size = state.brush_size;
                painter.brush_hardness = state.brush_hardness;
                painter.paint(x,y,canvas,state);
            }
        }   
        

        //UI
        ui.renderUI(state, canvas, w_width, w_height, canvas_width, canvas_height, res);

        state.attachShader(heightmap_shader);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(heightmap_shader);
    glDeleteProgram(terrain_shader);

    glfwTerminate();
    return 0;
}