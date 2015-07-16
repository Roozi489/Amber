#pragma once
#include "BaseComponent.h"
#include "Vector.h"

class TransformComponent : public BaseComponent
{
public:
    Vector3f position;
    Vector3f scale;
    
    float angle;
    Vector3f axis;
    // TODO: add rotation
    // Quaternion rotation;

    TransformComponent();
    TransformComponent(Vector3f pos, Vector3f scale);
    TransformComponent(Vector3f pos, Vector3f scale, float angle, Vector3f axis);
    ~TransformComponent();
};

