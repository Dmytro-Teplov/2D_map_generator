#include "serialization.h"


Archive::Archive()
{
}

const char* Archive::checkLastState()
{
    if (std::filesystem::exists(export_name)) {
        return "previous state exists";
    }
    return "no previous state detected";
}

bool Archive::startSerialization()
{
    serialization_started = true;

    output_file.open(export_name, std::ios::binary);// | std::ios::app);
    if (!output_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }

    return true;
}

bool Archive::stopSerialization()
{
    serialization_started = false;
    return false;
}

bool Archive::startDeserialization()
{
    deserialization_started = true;

    input_file.open(export_name, std::ios::binary);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }

    input_file.seekg(0, std::ios::end);
    std::streampos fileSize = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    file_contents.clear();
    file_contents.reserve(fileSize);
    input_file.read(file_contents.data(), fileSize);
    input_file.seekg(0, std::ios::beg);

    return true;
}

bool Archive::stopDeserialization()
{
    if (!deserialization_started)
        return false;

    deserialization_started = false;

    file_contents.clear();

    input_file.close();
    return false;
}

bool Archive::serialize(float data)
{
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return false;
    }
    output_file.write(reinterpret_cast<const char*>(&data), sizeof(float));

    return true;
}

bool Archive::serialize(int data)
{
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return false;
    }
    output_file.write(reinterpret_cast<const char*>(&data), sizeof(int));
    return true;
}

bool Archive::serialize(bool data)
{
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return false;
    }
    output_file.write(reinterpret_cast<const char*>(&data), sizeof(bool));
    return true;
}


bool Archive::serialize(glm::vec4 data)
{
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return false;
    }
    output_file.write(reinterpret_cast<const char*>(&data[0]), sizeof(float));
    output_file.write(reinterpret_cast<const char*>(&data[1]), sizeof(float));
    output_file.write(reinterpret_cast<const char*>(&data[2]), sizeof(float));
    output_file.write(reinterpret_cast<const char*>(&data[3]), sizeof(float));
    return true;
}

bool Archive::serialize(std::string data)
{
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return false;
    }
    output_file.write(reinterpret_cast<const char*>(&data), sizeof(std::string));
    return true;
}

bool Archive::serialize(unsigned char* texture_rgba, const int len)
{
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return false;
    }

    for (auto i = 0; i < len; i++)
    {
        int a = 0;
        a = texture_rgba[i];
        output_file.write(reinterpret_cast<const char*>(&a), sizeof(int));
    }
    return true;
}

bool Archive::serialize(FrameBuffer fb)
{
    int len = fb.getTextureLen();
    unsigned char* imageData = (unsigned char*)malloc(len);
    glBindTexture(GL_TEXTURE_2D, fb.getResultTexture());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return false;
    }

    for (auto i = 0; i < len; i++)
    {
        int a = 0;
        a = imageData[i];
        output_file.write(reinterpret_cast<const char*>(&a), sizeof(int));
    }
    return true;
}

bool Archive::deserialize(float& data)
{
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false; 
    }

    input_file.read(reinterpret_cast<char*>(&data), sizeof(float));
    return true;
}

bool Archive::deserialize(int& data)
{
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }

    input_file.read(reinterpret_cast<char*>(&data), sizeof(int));
    return true;
}

bool Archive::deserialize(bool& data)
{
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }

    input_file.read(reinterpret_cast<char*>(&data), sizeof(bool));
    return true;
}

bool Archive::deserialize(glm::vec4& data)
{
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }
    input_file.read(reinterpret_cast<char*>(&data[0]), sizeof(float));
    input_file.read(reinterpret_cast<char*>(&data[1]), sizeof(float));
    input_file.read(reinterpret_cast<char*>(&data[2]), sizeof(float));
    input_file.read(reinterpret_cast<char*>(&data[3]), sizeof(float));
    return true;
}
bool Archive::deserialize(std::string& data)
{
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }
    input_file.read(reinterpret_cast<char*>(&data), sizeof(std::string));
    return true;
}



bool Archive::deserialize(unsigned char*& texture_rgba,const int len)
{
    unsigned char* buffer = new unsigned char[len];
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }

    for (auto i = 0; i < len; i++)
    {
        int a = 0;
        input_file.read(reinterpret_cast<char*>(&a), sizeof(int));
        texture_rgba[i] = (unsigned char)a;
    }
  
 
    return true;
}

bool Archive::deserialize(FrameBuffer& fb, const int width, const int height)
{
    int len = width * height * 4;
    unsigned char* buffer = new unsigned char[len];
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << export_name << std::endl;
        return false;
    }

    for (auto i = 0; i < len; i++)
    {
        int a = 0;
        input_file.read(reinterpret_cast<char*>(&a), sizeof(int));
        buffer[i] = (unsigned char)a;
    }
    fb.fill(buffer);
    return true;
}
