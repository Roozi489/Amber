#include "Graphics//Model.h"

namespace Amber
{

Model::Model()
{
}

Model::Model(std::unique_ptr<Mesh> mesh)
{
	m_Meshes.push_back(std::move(mesh));
}

Model::~Model()
{
}

void Model::addMesh(std::unique_ptr<Mesh> mesh)
{
	m_Meshes.push_back(std::move(mesh));
}

}