
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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {

        glfwGetCursorPos(window, &state.last_x, &state.last_y);
        state.mouse_pressed = true;
        //std::cout << x << " " << y << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        state.mouse_pressed = false;
        state.view = glm::translate(state.view, state.transform);
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
    int w_width, w_height;
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


    ShaderProgramSource sources;

    sources = ParseShader("res/shaders/Terrain.shader");

    unsigned int shader = CreateShader(sources.vertexShader, sources.fragmentShader);

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current path is " << currentPath << std::endl;

    //Quad assets[128];
    Quad background(positions, 5, 5, indices, "res/assets/Proxy_map_2.png");
    background.initialize();
    background.debug();

    Quad house(positions_2, 5, 5, indices, "res/assets/Proxy_map_3.png");
    house.initialize();
    house.debug();
    Quad quad3(positions_3, 5, 5, indices, "res/assets/Proxy_map.png");
    quad3.initialize();
    quad3.debug();


    state.attachShader(shader);
    state.updMat(state.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");


    float res = (float)canvas_width / canvas_height;
    state.updFloat(res, "u_BgRes");

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

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwGetWindowSize(window, &w_width, &w_height);

        windowResizeHandler(w_width, w_height);
        state.projection = glm::scale(glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f), glm::vec3(state.zoom, state.zoom, state.zoom));
        state.updMat(state.projection, "projection");

        if (state.mouse_pressed)
        {
            glfwGetCursorPos(window, &x, &y);
            state.transform = glm::vec3(2) * glm::vec3((x - state.last_x)/ state.zoom, -(y - state.last_y) / state.zoom, 0.0f);
            glm::mat4 view_relative = glm::translate(state.view,state.transform);
            state.updMat(view_relative, "view");
            //house.draw();

        }

        background.draw();
        house.draw();
        quad3.draw();

        ImGui::Begin("Quests");
        ImGui::Text("Hello there adventurer! ");
        ImGui::SliderFloat("Color", &c, 0.0f, 100.0f);
        ImGui::SliderFloat("Size", &sz, 0.0f, 200.0f);

        ImGui::InputInt("canvas width", &canvas_width);
        ImGui::InputInt("canvas height", &canvas_height);
        if (ImGui::Button("Change the size"))
        {
            std::cout << w_width << " " << w_height << std::endl;
            std::cout << canvas_width << " " << canvas_height << std::endl;
            state.model = glm::scale(glm::mat4(1.0f), glm::vec3(canvas_width, canvas_height, 1.0f));
            state.updMat(state.model, "model");
            res = (float)canvas_width / canvas_height;
            state.updFloat(res, "u_BgRes");

        }
        ImGui::SliderFloat("Zoom", &state.zoom, 0.f, 25.f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}