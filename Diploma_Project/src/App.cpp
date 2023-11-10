
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

//#include "shader_helpers.cpp"

float zoom = 1.0f;
static bool mousePressed = false;
static double lastX = 0.0, lastY = 0.0;
bool mouse_pressed = false;
void windowResizeHandler(int window_width, int window_height) {
    glViewport(0, 0, window_width, window_height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    zoom += (float)yoffset*zoom/10.0f;
    if (zoom < 0.1f)
        zoom = 0.1f;
    if (zoom > 25.0f)
        zoom = 25.0f;
}
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);
        mouse_pressed = true;
        std::cout << x << " " << y << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mouse_pressed = false;
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
    //glm::mat4 view = glm::mat4(1.0f);
    //// note that we’re translating the scene in the reverse direction
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));



    float positions[20] = {
    -1.0f,  -1.0f, 0.0, 0.0, 1.0,
     1.0f,  -1.0f, 0.0, 1.0, 1.0,
     1.0f,   1.0f, 0.0, 1.0, 0.0,
    -1.0f,   1.0f, 0.0, 0.0, 0.0
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
    Quad background(shader, positions, 5, 5, indices, "res/assets/Proxy_map_2.png");
    
    background.initialize();
    background.debug();


    glm::mat4 model = glm::scale(glm::mat4(1.0f),glm::vec3(200.0f,100.0f,1.0f));
    background.updMat(model, "model");

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    background.updMat(view, "view");

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
    background.updMat(projection, "projection");
 

    float res = (float)canvas_width / canvas_height;
    background.updFloat(res, "u_BgRes");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float c = 1.0f;
    float sz = 100.0f;

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        background.changeColor(c);

        glfwGetWindowSize(window, &w_width, &w_height);

        windowResizeHandler(w_width, w_height);
        projection = glm::scale(glm::ortho(-(float)w_width , (float)w_width , -(float)w_height , (float)w_height , 0.1f, 100.0f), glm::vec3(zoom, zoom, zoom));
        background.updMat(projection, "projection");
        

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        

        background.draw();
        //house.draw();

        ImGui::Begin("Quests");
        ImGui::Text("Hello there adventurer! ");
        ImGui::SliderFloat("Color", &c, 0.0f, 100.0f);
        ImGui::SliderFloat("Size", &sz, 0.0f, 200.0f);
        //static int i0 = 123;
        ImGui::InputInt("canvas width", &canvas_width);
        ImGui::InputInt("canvas height", &canvas_height);
        if (ImGui::Button("Change the size"))
        {
            std::cout << w_width << " " << w_height << std::endl;
            std::cout << canvas_width << " " << canvas_height << std::endl;
            model = glm::scale(glm::mat4(1.0f), glm::vec3(canvas_width, canvas_height, 1.0f));
            background.updMat(model, "model");
            res = (float)canvas_width / canvas_height;
            background.updFloat(res, "u_BgRes");
            //std::cout << canvas_width << " " << canvas_height << std::endl;
            
        }
        ImGui::SliderFloat("Zoom", &zoom,0.f,25.f);
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