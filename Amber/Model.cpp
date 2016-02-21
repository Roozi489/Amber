#include "Model.h"

Model::Model()
{
}

Model::Model(std::unique_ptr<Mesh> mesh)
{
    mMeshes.push_back(std::move(mesh));
}

Model::~Model()
{
}

void Model::addMesh(std::unique_ptr<Mesh> mesh)
{
    mMeshes.push_back(std::move(mesh));
}