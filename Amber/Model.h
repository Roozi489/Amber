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
    Vector3f getPosition() const;
    void setPosition(Vector3f position);
    Vector3f getRotation() const;
    void move(Vector3f value);
    void rotate(Vector3f value);
    void rotate(float x, float y, float z);
    void setColor(Color color);
    void draw(Matrix4x4f& projectionMatrix, Matrix4x4f& viewMatrix);
private:
    Vector3f mPosition;
    Vector3f mRotation;
    std::vector<std::unique_ptr<Mesh>> mMeshes;
};

