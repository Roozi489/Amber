#pragma once

#include <GL/glew.h>

#include <string>
#include <map>

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    GLuint createProgram(const std::string& shaderName, const std::string& VertexShaderFilename, const std::string& FragmentShaderFilename);

    GLuint getShaderProgram(const std::string& programName);

private:
    GLuint createShader(GLenum shaderType, const std::string& shaderFileName);

    std::map<std::string, GLuint> shaderPrograms;
};

