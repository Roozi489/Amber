#include "ShaderManager.h"
#include "Utility.h"

#include <memory>

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
    for (auto& pair : mShaderPrograms)
    {
        GLuint pr = pair.second;
        glDeleteProgram(pr);
    }
    mShaderPrograms.clear();
}

GLuint ShaderManager::createShader(GLenum shaderType, const std::string& shaderFileName)
{
    int compileStatus;
    const char *source;

    std::string sourceString = loadFileToString("Shaders/" + shaderFileName);
    GLuint shaderID = glCreateShader(shaderType);
    source = sourceString.c_str();
    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        int logLenght = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLenght);
        std::unique_ptr<char[]> log(new char[logLenght]);
        glGetShaderInfoLog(shaderID, logLenght, nullptr, log.get());
        std::string message = "Failed to compile shader: " + shaderFileName + "\n";
        criticalError(message.append(log.get()));
    }

    return shaderID;
}

GLuint ShaderManager::createProgram(const std::string& shaderName, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
    int compileStatus;
    GLuint programID = glCreateProgram();
    glAttachShader(programID, createShader(GL_VERTEX_SHADER, vertexShaderFilename));
    glAttachShader(programID, createShader(GL_FRAGMENT_SHADER, fragmentShaderFilename));
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        int log_len = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &log_len);
        std::unique_ptr<char[]> log(new char[log_len]);
        glGetProgramInfoLog(programID, log_len, nullptr, log.get());
        std::string message = "Failed to link shader:\n";
        criticalError(message.append(log.get()));
    }
    mShaderPrograms[shaderName] = programID;

    return programID;
}

GLuint ShaderManager::getShaderProgram(const std::string& shaderName)
{
	assert_amber(mShaderPrograms.find(shaderName) != mShaderPrograms.end(), "ShaderProgram \"" + shaderName + "\" not found.");
    return mShaderPrograms[shaderName];
}