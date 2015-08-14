#pragma once
#include "Utility.h"
#include "Material.h"
#include "Texture.h"
#include "Vector.h"
#include "Matrix.h"

#include "GL/glew.h"

#include <string>
#include <vector>

class Mesh
{
public:
    std::vector<Vector3f> vertices;
    std::vector<Vector2f> uvs;
    std::vector<Vector3f> normals;
    std::vector<unsigned short> indices;

    // TODO: Texture manager
    bool hasTexture;
    Texture* texture;

    GLuint vao;

    GLuint vertexVBO;
    GLuint uvVBO;
    GLuint normalVBO;
    GLuint elementVBO;

    Mesh();
    ~Mesh();

    bool loadGLData();
	void setTexture(const std::string& name);

    float boundingSphereRadiusFast();

    std::string toString();

};