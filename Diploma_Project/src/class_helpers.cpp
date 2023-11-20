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
void Quad::initialize()
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
void StateHandler::attachShader(unsigned int shader_)
{
    shader = shader_;
    glUseProgram(shader);
    std::cout << glGetError();
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
