#include "Core/Mesh.h"
#include "Core/Globals.h"

#include "GL/glew.h"

#include <sstream>

namespace Amber
{

Mesh::Mesh()
	: hasTexture(false)
{
}

Mesh::~Mesh()
{
}

void Mesh::setVaoAndVbo()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (meshComponents & MeshComponents::Vertex)
	{
		glGenBuffers(1, &vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (meshComponents & MeshComponents::TexCoord)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(Vector2f), uvs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (meshComponents & MeshComponents::Normal)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (meshComponents & MeshComponents::Index)
	{
		glGenBuffers(1, &elementVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
	}
}

void Mesh::destroy()
{
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &elementVBO);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::setTexture(const std::string& name)
{
	texture = g_textureManager.getTexture(name);
	hasTexture = true;
}

void Mesh::calculateBoundingSphereRadiusFast()
{
	float max = 0.f;
	for (auto& v : vertices)
	{
		float length = v.length();
		if (length > max)
			max = length;
	}
	m_BoundingSphereRadius = max;
}

float Mesh::getBoundingSphereRadiusFast()
{
	return m_BoundingSphereRadius;
}

std::string Mesh::toString()
{
	std::stringstream s;
	s << "Vertices count: " << vertices.size() << std::endl;
	s << "Normals count: " << normals.size() << std::endl;
	s << "UVs count: " << uvs.size() << std::endl;

	return s.str();
}

}