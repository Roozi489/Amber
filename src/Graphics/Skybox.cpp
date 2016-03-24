#include "Graphics/Skybox.h"
#include "Core/Globals.h"

#include <GL/glew.h>

namespace Amber
{

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
}

void Skybox::init()
{
    float points[] = 
    {
        -10.0f, 10.0f, -10.0f,
        -10.0f, -10.0f, -10.0f,
        10.0f, -10.0f, -10.0f,
        10.0f, -10.0f, -10.0f,
        10.0f, 10.0f, -10.0f,
        -10.0f, 10.0f, -10.0f,

        -10.0f, -10.0f, 10.0f,
        -10.0f, -10.0f, -10.0f,
        -10.0f, 10.0f, -10.0f,
        -10.0f, 10.0f, -10.0f,
        -10.0f, 10.0f, 10.0f,
        -10.0f, -10.0f, 10.0f,

        10.0f, -10.0f, -10.0f,
        10.0f, -10.0f, 10.0f,
        10.0f, 10.0f, 10.0f,
        10.0f, 10.0f, 10.0f,
        10.0f, 10.0f, -10.0f,
        10.0f, -10.0f, -10.0f,

        -10.0f, -10.0f, 10.0f,
        -10.0f, 10.0f, 10.0f,
        10.0f, 10.0f, 10.0f,
        10.0f, 10.0f, 10.0f,
        10.0f, -10.0f, 10.0f,
        -10.0f, -10.0f, 10.0f,

        -10.0f, 10.0f, -10.0f,
        10.0f, 10.0f, -10.0f,
        10.0f, 10.0f, 10.0f,
        10.0f, 10.0f, 10.0f,
        -10.0f, 10.0f, 10.0f,
        -10.0f, 10.0f, -10.0f,

        -10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f, 10.0f,
        10.0f, -10.0f, -10.0f,
        10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f, 10.0f,
        10.0f, -10.0f, 10.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof (float), &points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    skyboxTexture.loadFromFile("Textures/negz.jpg", "Textures/posz.jpg", "Textures/posy.jpg", "Textures/negy.jpg", "Textures/negx.jpg", "Textures/posx.jpg");

    shaderProgramID = g_shaderManager.createProgram("skybox", "skybox.vert", "skybox.frag")->handle;

    checkGlError();
}

void Skybox::destroy()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Skybox::update()
{
    glUseProgram(shaderProgramID);

    // TODO: add support for proper camera rotation
    //Matrix4x4f vMat = Matrix4x4f::rotate(Matrix4x4f::identity(), (g_camera.getRotationY() + PI_HALF), Vector3f(0.f, 1.f, 0.f));

    GLuint vLoc = glGetUniformLocation(shaderProgramID, "V");
    //glUniformMatrix4fv(vLoc, 1, GL_FALSE, &vMat[0][0]);

    Matrix4x4f pMat = g_camera.getProjectionMatrix();
    GLuint pLoc = glGetUniformLocation(shaderProgramID, "P");
    glUniformMatrix4fv(pLoc, 1, GL_FALSE, &pMat[0][0]);
}

void Skybox::render()
{
    glDepthMask(GL_FALSE);
	g_shaderManager.getShaderProgram("skybox")->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.textureHandle);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

}