#include "Graphics/GL/Mesh.h"
#include "Core/Globals.h"

#include "GL/glew.h"

#include <sstream>

namespace Amber
{

Mesh::Mesh()
	: hasTexture(false)
	, id(generateMeshID())
{
}

Mesh::~Mesh()
{
}

void Mesh::setVaoAndVbo()
{
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	if (meshComponents & MeshComponents::VertexComponent)
	{
		glGenBuffers(1, &vboHandles[VBOIndex::Vertex]);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBOIndex::Vertex]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(VSLocation::Vertex);
		glVertexAttribPointer(VSLocation::Vertex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	if (meshComponents & MeshComponents::TexCoord)
	{
		glGenBuffers(1, &vboHandles[VBOIndex::TexCoord]);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBOIndex::TexCoord]);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(Vector2f), uvs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(VSLocation::TexCoord);
		glVertexAttribPointer(VSLocation::TexCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	if (meshComponents & MeshComponents::Normal)
	{
		glGenBuffers(1, &vboHandles[VBOIndex::Normal]);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBOIndex::Normal]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(VSLocation::Normal);
		glVertexAttribPointer(VSLocation::Normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	if (meshComponents & MeshComponents::Index)
	{
		glGenBuffers(1, &vboHandles[VBOIndex::Element]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[VBOIndex::Element]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16), indices.data(), GL_STATIC_DRAW);
	}
	if (instanced)
	{
		glGenBuffers(1, &vboHandles[VBOIndex::Orientation]);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBOIndex::Orientation]);
		glEnableVertexAttribArray(VSLocation::Orientation);
		glVertexAttribPointer(VSLocation::Orientation, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4f), nullptr);
		glVertexAttribDivisor(VSLocation::Orientation, 1);

		glGenBuffers(1, &vboHandles[VBOIndex::MVP]);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBOIndex::MVP]);

		for (uint32 i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(VSLocation::MVP + i);
			glVertexAttribPointer(VSLocation::MVP + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4x4f), (const GLvoid*)(sizeof(Vector4f) * i));
			glVertexAttribDivisor(VSLocation::MVP + i, 1);
		}

		glGenBuffers(1, &vboHandles[VBOIndex::Color]);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBOIndex::Color]);
		glEnableVertexAttribArray(VSLocation::Color);
		glVertexAttribPointer(VSLocation::Color, 4, GL_FLOAT, GL_FALSE, sizeof(Color), nullptr);
		glVertexAttribDivisor(VSLocation::Color, 1);
	}
}

void Mesh::destroy()
{
	if (meshComponents & MeshComponents::VertexComponent)
		glDeleteBuffers(1, &vboHandles[VBOIndex::Vertex]);
	if (meshComponents & MeshComponents::TexCoord)
		glDeleteBuffers(1, &vboHandles[VBOIndex::TexCoord]);
	if (meshComponents & MeshComponents::Normal)
		glDeleteBuffers(1, &vboHandles[VBOIndex::Normal]);
	if (meshComponents & MeshComponents::Index)
		glDeleteBuffers(1, &vboHandles[VBOIndex::Element]);
	if (instanced)
	{
		glDeleteBuffers(1, &vboHandles[VBOIndex::Orientation]);
		for (uint32 i = 0; i < 4; ++i)
			glDeleteBuffers(VSLocation::MVP + i, &vboHandles[VBOIndex::Orientation]);
		glDeleteBuffers(1, &vboHandles[VBOIndex::Color]);
	}
	glDeleteVertexArrays(1, &vaoHandle);
}

void Mesh::setTexture(const std::string& name)
{
	texture = g_textureManager.getTexture(name);
	hasTexture = true;
}

void Mesh::calculateBoundingSphereRadiusFast()
{
	float max = 0.f;
	for (const auto& v : vertices)
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

MeshID Mesh::generateMeshID()
{
	static MeshID id = 0;
	return id++;
}

}