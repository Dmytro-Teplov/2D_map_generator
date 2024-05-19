#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <vector>
#include <glm.hpp>
#include "GL_helpers.h"
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <stdexcept>
//#include <GL/glew.h>
//#include <cassert>


class Archive
{
	char export_name[256] = "map_project_1.bin";
	bool serialization_started = false;
	bool deserialization_started = false;
	std::ifstream input_file;
	std::ofstream output_file;
	std::vector<char> file_contents;
	
public:
	
	Archive();
	Archive(const char* project_name);

	const char* checkLastState();

	bool startSerialization();
	bool stopSerialization();

	bool startDeserialization();
	bool stopDeserialization();

	bool serialize(float data);
	bool serialize(int data);
	bool serialize(bool data);
	bool serialize(glm::vec4 data);
	bool serialize(std::string data);
	bool serialize(unsigned char* texture_rgba, const int len);
	bool serialize(FrameBuffer fb);

	bool deserialize(float &data);
	bool deserialize(int &data);
	bool deserialize(bool &data);
	bool deserialize(glm::vec4& data);
	bool deserialize(std::string& data);
	bool deserialize(unsigned char*& texture_rgba, const int len);
	bool deserialize(FrameBuffer& fb, const int width, const int height);
	
	
};