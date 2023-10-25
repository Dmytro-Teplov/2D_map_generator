#define STB_IMAGE_IMPLEMENTATION

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stb_image.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
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
    float vertices[20];
    float transform[16];
    int position_stride;
    int uv_stride;

    unsigned int indices[6];
    const char* texture_path;
    int color_loc;
    int text_loc;
    int size_loc;
    int proj_loc;
    Quad()
    {
        shader = -1;
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
    void initialize()
    {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, std::size(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * position_stride, (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * uv_stride, (void*)(sizeof(float) * position_stride - sizeof(float) * 2));//2 is the amount of uv coordinates

        unsigned int indBuffer;
        glGenBuffers(1, &indBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, std::size(indices) * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        // Load the image data
        int channels;
        int width, height;
        unsigned char* imageData = stbi_load(texture_path, &width, &height, &channels, 4);
        if (imageData == nullptr) {
            std::cout << "fuck";
            stbi_image_free(imageData);
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
    //void changeSize(float multiplier)
    //{

    //    for (int i = 0; i < 16; i++) {
    //        transform[i] = vertices[i]*multiplier;
    //    }
    //    use_transform = true;
    //    //glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), transform, GL_STATIC_DRAW);
    //    //glUniform1f(size_loc, multiplier);
    //}
    void changeSize(float canvas_ratio, bool w_ratio)
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

        //glBufferData(GL_ARRAY_BUFFER, std::size(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);
        //glUniform1f(size_loc, multiplier);
    }
    void updMat(glm::mat4 matrix, const char* matrix_name)
    {
        int location = glGetUniformLocation(shader, matrix_name);

        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void debug()
    {
        std::cout << vertices[0] << " " << vertices[7] << std::endl;
        std::cout << indices[0] << " " << indices[5] << std::endl;
        std::cout << texture_path;
    }
    void draw()
    {
        /*if(use_transform)
            glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), transform, GL_STATIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);*/
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
};

class WindowHandler
{
    GLFWwindow* window;
    int w_width, w_height;
    float canvas_aspect_ratio = 1.0f;
    Quad assets[128];

    WindowHandler(GLFWwindow& window_)
    {
        window = &window_;
        glfwGetWindowSize(window, &w_width, &w_height);

    }
    void setAspectRatio(float ratio)
    {
        canvas_aspect_ratio = ratio;
    }
    void canvasUpdate()
    {
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        if ((abs(window_width - w_width) > 1 || abs(window_height - w_height) > 1))
        {
            glViewport(0, 0, window_width * canvas_aspect_ratio, window_height);
        }
    }
};
void canvasUpdate(GLFWwindow* window, int width, int height)
{
    //int window_width, window_height;
    glfwGetWindowSize(window, &width, &height);
}

void windowResizeHandler(int window_width, int window_height) {
    // Adjust the viewport
    /*float aspect_ratio_canvas = (float)canvas_h / canvas_w;
    float aspect_ratio_window = (float)window_height / window_width;
    if (aspect_ratio_window < 1.0 && aspect_ratio_canvas < 1.0)
    {
        glViewport(0, 0, window_height * aspect_ratio_canvas, window_height);
    }
    else if (aspect_ratio_window > 1.0 && aspect_ratio_canvas > 1.0)
    {
        glViewport(0, 0, window_width, window_width * aspect_ratio_canvas);
    }
    else if (aspect_ratio_window < 1.0 && aspect_ratio_canvas > 1.0)
    {
        glViewport(0, 0, window_width, window_width * aspect_ratio_canvas);
    }*/

    glViewport(0, 0, window_width, window_height);


}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    int w_width, w_height;
    float canvas_aspect_ratio = 1.0;
    int canvas_width = 640, canvas_height = 480;
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
    /*float positions2[8] = {
     0.0f,  0.0f,
     1.0f,  0.0f,
     1.0f,  1.0f,
     0.0f,  1.0f
    };*/

    ShaderProgramSource sources;

    sources = ParseShader("res/shaders/RedTris.shader");

    unsigned int shader = CreateShader(sources.vertexShader, sources.fragmentShader);

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current path is " << currentPath << std::endl;
    Quad assets[128];
    Quad background(shader, positions, 5, 5, indices, "res/assets/Map.png");
    assets[0] = background;
    assets[0].initialize();
    assets[0].debug();


    glm::mat4 model = glm::mat4(1.0f);
    assets[0].updMat(model, "model");

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    assets[0].updMat(view, "view");

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
    assets[0].updMat(projection, "projection");
    //Quad house(shader, positions2, indices, "res/assets/Map.png");
    //house.initialize();
    //



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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


        assets[0].changeColor(c);
        //assets[0].changeSize(sz);

        glfwGetWindowSize(window, &w_width, &w_height);
        windowResizeHandler(w_width, w_height);
        projection = glm::ortho(-(float)w_width / 100, (float)w_width / 100, -(float)w_height / 100, (float)w_height / 100, 0.1f, 100.0f);
        model = glm::scale(glm::mat4(1.0f), glm::vec3(sz, sz, sz));
        assets[0].updMat(model, "model");
        assets[0].updMat(projection, "projection");

        //assets[0].updProj(projection);

        assets[0].draw();
        //house.draw();

        ImGui::Begin("Quests");
        ImGui::Text("Hello there adventurer! ");
        ImGui::SliderFloat("Color", &c, 0.0f, 1.0f);
        ImGui::SliderFloat("Size", &sz, 0.0f, 2.0f);
        //static int i0 = 123;
        ImGui::InputInt("canvas width", &canvas_width);
        ImGui::InputInt("canvas height", &canvas_height);
        if (ImGui::Button("Change the size"))
        {
            model = glm::scale(glm::mat4(1.0f), glm::vec3(sz, sz, sz));
            assets[0].updMat(model, "model");
        }
        ImGui::InputFloat("aspect ratio", &canvas_aspect_ratio);
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