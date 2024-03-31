#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <GL/glew.h>
#include <cassert>


struct ShaderProgramSource
{
	std::string vertexShader;
	std::string fragmentShader;
};
ShaderProgramSource ParseShader(const std::string& source);
unsigned int CompileShader(unsigned int type, const std::string& source);
unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

class VertexBuffer
{
private:
	unsigned int v_bufferID = 0;
	void Release();

public:
	VertexBuffer(const void* data, unsigned int size) noexcept;
	VertexBuffer();
	~VertexBuffer() { Release(); };

	void initialize(const void* data, unsigned int size);
	void bind() const;
	void unbind() const;

	VertexBuffer operator=(const VertexBuffer& vb);
};

class IndexBuffer
{
private:
	unsigned int i_bufferID = 0;
	unsigned int indCount = 0;
	void Release();
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	IndexBuffer();
	~IndexBuffer() { Release(); };

	void initialize(const unsigned int* data, unsigned int count);
	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return indCount; }

	IndexBuffer operator=(const IndexBuffer& ib);
};

class FrameBuffer
{
private:
	unsigned int fb_ID = 0;
	unsigned int texture_ID = 0;
	unsigned int shader_ID = 0;
	int w_width = 640;
	int w_height = 480;
public:
	FrameBuffer(int w_width_, int w_height_);
	void bind();
	void unBind();
	void updateSize(int w_width_, int w_height_);
	unsigned int getResultTexture();
	unsigned int getFbID();
	
	//Create fb class and then create fb to colorized map to.
};