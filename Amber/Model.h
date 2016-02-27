#pragma once
#include "Mesh.h"

#include <vector>
#include <memory>

class Model
{
public:
    Model();
    Model(std::unique_ptr<Mesh> mesh);
    ~Model();

    void addMesh(std::unique_ptr<Mesh> mesh);
private:
    std::vector<std::unique_ptr<Mesh>> m_Meshes;
};

