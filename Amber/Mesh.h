#pragma once
#include "Texture.h"
#include "Vector.h"
#include "OpenGL.h"

#include <string>
#include <vector>

enum MeshComponents
{
	Vertex = 1,
	TexCoord = 2,
	Normal = 4,
	Index = 8,
	VertexUVNormalIndex = Vertex | TexCoord | Normal | Index,
	VertexIndex = Vertex | Index
};

class Mesh
{
public:
	MeshComponents meshComponents;

    std::vector<Vector3f> vertices;
    std::vector<Vector2f> uvs;
    std::vector<Vector3f> normals;
    std::vector<unsigned short> indices;

    // TODO: Material manager
    bool hasTexture;
    Texture* texture;

    GLuint vao;

    GLuint vertexVBO;
    GLuint texCoordVBO;
    GLuint normalVBO;
    GLuint elementVBO;

    Mesh();
    ~Mesh();

    void setVaoAndVbo();
	void destroy();
	void setTexture(const std::string& name);

    float boundingSphereRadiusFast();

    std::string toString();
};