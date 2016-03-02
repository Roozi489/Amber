#include "Managers/MeshManager.h"
#include "Window/Window.h"

#include <memory>
#include <fstream>

namespace Amber
{

struct FaceIndexVertTexNorm
{
	unsigned int v0, t0, n0;
	unsigned int v1, t1, n1;
	unsigned int v2, t2, n2;
};

MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
}

bool MeshManager::preloadMesh(const std::string& fileName)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<Vector3f> temp_vertices;
	std::vector<Vector2f> temp_uvs;
	std::vector<Vector3f> temp_normals;

	std::ifstream file("Models/" + fileName);
	if (!file.is_open())
	{
		criticalError("MeshManager::loadModel - Error opening file " + fileName);
		return false;
	}

	while (!file.fail())
	{
		std::string prefix;
		file >> prefix;

		if (prefix == "v")
		{
			Vector3f v;
			file >> v.x >> v.y >> v.z;
			temp_vertices.push_back(v);
		}
		else if (prefix == "vt")
		{
			Vector2f vt;
			file >> vt.x >> vt.y;
			temp_uvs.push_back(vt);
		}
		else if (prefix == "vn")
		{
			Vector3f vn;
			file >> vn.x >> vn.y >> vn.z;
			temp_normals.push_back(vn);
		}
		else if (prefix == "f")
		{
			FaceIndexVertTexNorm t;
			char slash;
			// TODO: allow support of no uv coords (7//7)
			file >> t.v0 >> slash >> t.t0 >> slash >> t.n0;
			file >> t.v1 >> slash >> t.t1 >> slash >> t.n1;
			file >> t.v2 >> slash >> t.t2 >> slash >> t.n2;

			vertexIndices.push_back(t.v0);
			vertexIndices.push_back(t.v1);
			vertexIndices.push_back(t.v2);
			uvIndices.push_back(t.t0);
			uvIndices.push_back(t.t1);
			uvIndices.push_back(t.t2);
			normalIndices.push_back(t.n0);
			normalIndices.push_back(t.n1);
			normalIndices.push_back(t.n2);
		}
		else
		{
			// Other cases
		}
		file.ignore(1000, '\n');
	}

	file.close();

	auto mesh = std::make_unique<Mesh>();
	mesh->meshComponents = MeshComponents::VertexUVNormalIndex;

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		Vector3f vertex = temp_vertices[vertexIndex - 1];
		Vector2f uv = temp_uvs[uvIndex - 1];
		Vector3f normal = temp_normals[normalIndex - 1];

		mesh->vertices.push_back(vertex);
		mesh->uvs.push_back(uv);
		mesh->normals.push_back(normal);
		mesh->indices.push_back(i);
	}

	// TODO: remove this when its not necessary
	mesh->calculateBoundingSphereRadiusFast();
	mesh->setVaoAndVbo();

	meshes.insert(std::make_pair(fileName, std::move(mesh)));
	return true;
}

Mesh* MeshManager::getMesh(const std::string& name)
{
	if (meshes.find(name) == meshes.end())
	{
		preloadMesh(name);
	}

	return meshes[name].get();
}

}