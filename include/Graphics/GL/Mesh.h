#pragma once
#include "Graphics/GL/Texture.h"
#include "Math/Vector.h"
#include "Graphics/GL/OpenGL.h"

#include <string>
#include <vector>

namespace Amber
{

enum MeshComponents
{
	VertexComponent = 1,
	TexCoord = 2,
	Normal = 4,
	Index = 8,
	VertexUVNormalIndex = VertexComponent | TexCoord | Normal | Index,
	VertexIndex = VertexComponent | Index
};

namespace VBOIndex
{
enum Index
{
	Vertex,
	TexCoord,
	Normal,
	Element,
	Orientation,
	MVP,
	Color,
	Count
};
}
namespace VSLocation
{
enum Location
{
	Vertex = 0,
	TexCoord = 1,
	Normal = 2,
	Orientation = 3,
	MVP = 4,
	Color = 8
};
}

using MeshID = uint32;

class Mesh
{
public:
	MeshComponents meshComponents;

	std::vector<Vector3f> vertices;
	std::vector<Vector2f> uvs;
	std::vector<Vector3f> normals;
	std::vector<uint16> indices;

	bool instanced = true;

	MeshID id;
	std::string name;

	// TODO: Material manager
	bool hasTexture;
	Texture* texture;

	GLuint vaoHandle;
	GLuint vboHandles[VBOIndex::Count];

	Mesh();
	~Mesh();

	void setVaoAndVbo();
	void destroy();
	void setTexture(const std::string& name);

	// TODO: make this automatic
	void calculateBoundingSphereRadiusFast();
	float getBoundingSphereRadiusFast();

	std::string toString();

private:
	float m_BoundingSphereRadius;

	static MeshID generateMeshID();
};

}