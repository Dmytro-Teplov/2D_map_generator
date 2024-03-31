#include "GL_helpers.h"


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // Windows-specific debugging mechanism
    #define DEBUG_BREAK __debugbreak()
#else
    // Use assert for other operating systems
    #define DEBUG_BREAK assert(false)
#endif

// Usage:
#define ASSERT(x) if(!(x)) DEBUG_BREAK;

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

void VertexBuffer::initialize(const void* data, unsigned int size)
{
    GLCall(glGenBuffers(1, &v_bufferID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, v_bufferID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, v_bufferID);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer VertexBuffer::operator=(const VertexBuffer& vb)
{
    this->v_bufferID = vb.v_bufferID;
    return VertexBuffer();
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

void IndexBuffer::initialize(const unsigned int* data, unsigned int count)
{
    glGenBuffers(1, &i_bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_bufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

void IndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_bufferID);
}

void IndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer IndexBuffer::operator=(const IndexBuffer& ib)
{
    this->i_bufferID = ib.i_bufferID;
    this->indCount = ib.indCount;
    return IndexBuffer();
}

FrameBuffer::FrameBuffer(int w_width_, int w_height_)
{
    w_width = w_width_;
    w_height = w_height_;
    glGenTextures(1, &texture_ID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_width, w_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &fb_ID);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_ID);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        std::cout << "frame buffer is done\n";
    else
        std::cout << "error setting up frame buffer\n";
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_ID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::bind()
{
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_width, w_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_ID);
}
void FrameBuffer::unBind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_width, w_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::updateSize(int w_width_, int w_height_)
{
    w_width = w_width_;
    w_height = w_height_;
}

unsigned int FrameBuffer::getResultTexture()
{
    return texture_ID;
}
unsigned int FrameBuffer::getFbID()
{
    return fb_ID;
}
