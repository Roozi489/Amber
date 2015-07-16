#pragma once
#include "Utility.h"
#include "Material.h"
#include "Texture2D.h"
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
    Texture2D texture;

    GLuint vao;

    GLuint vertexVBO;
    GLuint uvVBO;
    GLuint normalVBO;
    GLuint elementVBO;

    Mesh();
    ~Mesh();

    bool loadGLData();
    void loadTexture(std::string filename);
    void changeTexture(Texture2D newTexture);

    float boundingSphereRadiusFast();

    std::string toString();

};