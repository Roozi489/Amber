#pragma once
#include "Utility.h"
#include "Material.h"
#include "Texture.h"
#include "Vector.h"

#include "GL/glew.h"

#include <string>
#include <vector>

enum MeshComponents
{
	Vertex = 1,
	UV = 2,
	Normal = 4,
	Index = 8,
	VertexUVNormalIndex = Vertex | UV | Normal | Index,
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
    GLuint uvVBO;
    GLuint normalVBO;
    GLuint elementVBO;

    Mesh();
    ~Mesh();

    bool setVaoAndVbo();
	void setTexture(const std::string& name);

    float boundingSphereRadiusFast();

    std::string toString();
};