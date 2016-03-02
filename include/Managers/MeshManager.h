#pragma once
#include "Core/Mesh.h"

#include <unordered_map>
#include <string>
#include <memory>

namespace Amber
{

class MeshManager
{
public:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;

	MeshManager();
	~MeshManager();

	bool preloadMesh(const std::string& fileName);
	Mesh* getMesh(const std::string& name);
};

}