#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
//#include "renderer.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

struct ShaderProgramScource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, unsigned int> m_UniformLocationCache;
public:
    Shader(const std::string &filename);
    ~Shader();

    void bind() const;
    void unbind() const;

    // set uniforms
    void setUniform4f(const std::string &name, float v0, float v1, float v2, float v3);
private:
    ShaderProgramScource parseShader(const std::string filepath);
    unsigned int compileShader(const std::string &source, unsigned int type);
    unsigned int getUniformLocation(const std::string &name);
    unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader);
};

#endif // SHADER_H
