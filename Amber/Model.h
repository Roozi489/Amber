#pragma once
#include "Utility.h"
#include "Mesh.h"
#include "Matrix.h"

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
    std::vector<std::unique_ptr<Mesh>> mMeshes;
};

