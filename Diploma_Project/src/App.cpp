#define STB_IMAGE_IMPLEMENTATION

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stb_image.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>


struct ShaderProgramSource
{
    std::string vertexShader;
    std::string fragmentShader;
};

static ShaderProgramSource ParseShader(const std::string& source)
{
    std::ifstream stream(source);
    std::string line;

    int none = -1, vertex = 0, fragment = 1;
    int type = none;

    std::string shaders[2] = { "","" };

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = vertex;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = fragment;
            }
        }
        else
        {
            shaders[type] += line + "\n";
        }
    }
    return { shaders[vertex], shaders[fragment] };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    //err
    int result;

    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

struct Quad
{
    unsigned int shader;
    float positions[8];
    float transform[8];
    unsigned int indices[6];
    const char * texture_path;
    int color_loc;
    int text_loc;
    int size_loc;
    Quad(unsigned int shader_,float positions_[8], unsigned int indices_[6], const char * texture_path_)
    {
        shader = shader_;
        for (int i = 0; i < 8; i++) {
            positions[i] = positions_[i];
            if (i < 6)
            {
                indices[i] = indices_[i];
            }
        }
        texture_path = texture_path_;

    }
    void initialize()
    {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

        unsigned int indBuffer;
        glGenBuffers(1, &indBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        // Load the image data
        int channels;
        int width, height;
        unsigned char* imageData = stbi_load(texture_path, &width, &height, &channels, 4);
        if (imageData == nullptr) {
            std::cout << "fuck";
        }

        unsigned int texture;
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
        std::cout << glGetError();
        // Unbind the texture object
        //glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(shader);
        std::cout << glGetError();
        int location = glGetUniformLocation(shader, "u_Color");
        int textLoc = glGetUniformLocation(shader, "texture1");
        int szLoc = glGetUniformLocation(shader, "size");
        std::cout << glGetError();
        //ASSERT(location != 1);
        glUniform4f(location, 0.5f, 0.0f, 0.5f, 1.0f);
        glUniform1i(textLoc, 0);
        float col = 0.0f;
        color_loc = location;
        text_loc = textLoc;
        size_loc = szLoc;
    }
    void changeColor(float col)
    {
        glUniform4f(color_loc, col, col, col, 1.0f);
    }
    void changeSize(float multiplier)
    {

        for (int i = 0; i < 8; i++) {
            transform[i] = positions[i]*multiplier;
        }
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), transform, GL_STATIC_DRAW);
        //glUniform1f(size_loc, multiplier);
    }
    void debug()
    {
        std::cout << positions[0] << " " << positions[7] << std::endl;
        std::cout << indices[0] << " " << indices[5] << std::endl;
        std::cout << texture_path;
    }
    void draw()
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
};

void windowResizeHandler(int windowWidth, int windowHeight) {
    // Adjust the viewport
    glViewport(0, 0, windowWidth, windowHeight);

    // Adjust the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

    // Switch back to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    /* Create a windowed mode window and its OpenGL context */
    int w_width, w_height;
    window = glfwCreateWindow(640, 480, "Magic Maps", NULL, NULL);
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
    float positions[8] = {
    -1.0f,  -1.0f,
     1.0f,  -1.0f,
     1.0f,  1.0f,
    -1.0f,  1.0f
    };

    unsigned int indices[6] = {
    0,1,2,
    2,3,0
    };
    float positions2[8] = {
    -0.8f,  -0.8f,
     0.8f,  -0.8f,
     0.8f,  0.8f,
    -0.8f,  0.8f
    };
    
    ShaderProgramSource sources;

    sources = ParseShader("res/shaders/RedTris.shader");

    unsigned int shader = CreateShader(sources.vertexShader, sources.fragmentShader);

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current path is " << currentPath << std::endl;

    Quad background(shader, positions, indices, "res/assets/Map.png");
    background.initialize();

    background.debug();
   

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float c = 1.0f;
    float sz = 1.0f;

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        background.changeColor(c);
        background.changeSize(sz);

        glfwGetWindowSize(window, &w_width, &w_height);
        windowResizeHandler(w_width, w_height);
     
        background.draw();
 
        ImGui::Begin("Quests");
        ImGui::Text("Hello there adventurer! ");
        ImGui::SliderFloat("Color", &c, 0.0f, 1.0f);
        ImGui::SliderFloat("Size", &sz, 0.0f, 2.0f);
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