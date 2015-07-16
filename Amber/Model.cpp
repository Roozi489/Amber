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

Vector3f Model::getPosition() const
{
    return mPosition;
}

void Model::setPosition(Vector3f value)
{
    mPosition = value;
}

Vector3f Model::getRotation() const
{
    return mRotation;
}

void Model::move(Vector3f value)
{
    mPosition = mPosition + value;
}

void Model::rotate(Vector3f value)
{
    rotate(value.x, value.y, value.z);
}

void Model::rotate(float x, float y, float z)
{
    mRotation.x = float(int(mRotation.x + x) % 360);
    mRotation.y = float(int(mRotation.y + y) % 360);
    mRotation.z = float(int(mRotation.z + z) % 360);
}

void Model::draw(Matrix4x4f& projectionMatrix, Matrix4x4f& viewMatrix)
{
    /*modelMatrix = glm::rotate(modelMatrix, mRotation.x, Vector3f(1.f, 0.f, 0.f));
    modelMatrix = glm::rotate(modelMatrix, mRotation.y, Vector3f(0.f, 1.f, 0.f));
    modelMatrix = glm::rotate(modelMatrix, mRotation.z, Vector3f(0.f, 0.f, 1.f));*/
    //for (auto& mesh : mMeshes)
        //mesh->draw(TODO, modelMatrix, projectionMatrix, viewMatrix);
}