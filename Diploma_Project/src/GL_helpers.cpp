#include "GL_helpers.h"
#include <GL/glew.h>

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

ShaderProgramSource ParseShader(const std::string& source)
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

unsigned int CompileShader(unsigned int type, const std::string& source)
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

unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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


void VertexBuffer::Release()
{
    glDeleteBuffers(1, &v_bufferID);
    v_bufferID = 0;
}



VertexBuffer::VertexBuffer(const void* data, unsigned int size) noexcept
{
    GLCall(glGenBuffers(1, &v_bufferID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, v_bufferID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer() 
{
    v_bufferID = 0;
}



void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, v_bufferID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    :indCount(count)
{
    glGenBuffers(1, &i_bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_bufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW);
}
IndexBuffer::IndexBuffer()
{
    i_bufferID = 0;
}


void IndexBuffer::Release()
{
    glDeleteBuffers(1, &i_bufferID);
    i_bufferID = 0;
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_bufferID);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
