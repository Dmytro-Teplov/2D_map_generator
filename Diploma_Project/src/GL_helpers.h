#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>


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
};

