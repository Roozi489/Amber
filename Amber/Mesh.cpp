#include "Mesh.h"
#include "Globals.h"

#include "GL/glew.h"

#include <map>
#include <sstream>

Mesh::Mesh()
    : hasTexture(false)
{
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertexVBO);
    glDeleteBuffers(1, &uvVBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteVertexArrays(1, &vao);
}

bool Mesh::loadGLData()
{   
    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Upload data to buffer
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &uvVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(Vector2f), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // The same for indices
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

    checkGlError();

    return true;
}

void Mesh::setTexture(const std::string& name)
{
	texture = gTextureManager.getTexture(name);
    hasTexture = true;
}

float Mesh::boundingSphereRadiusFast()
{
    float max = 0.f;
    for (auto& v : vertices)
    {
        float length = v.length();
        if (length > max)
            max = length;
    }
    return max;
}

std::string Mesh::toString()
{
    std::stringstream s;
    s << "Vertices count: " << vertices.size() << std::endl;
    s << "Normals count: " << normals.size() << std::endl;
    s << "UVs count: " << uvs.size() << std::endl;

    return s.str();
}
