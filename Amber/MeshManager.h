#pragma once
#include "Model.h"

#include <unordered_map>
#include <string>
#include <memory>

class MeshManager
{
public:
    std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;

    MeshManager();
    ~MeshManager();

    bool loadMesh(const std::string& fileName, bool loadGLData = true);
    Mesh* getMesh(const std::string& name, bool loadGLData = true);
};

