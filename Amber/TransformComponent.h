#pragma once
#include "BaseComponent.h"
#include "Vector.h"
#include "Quaternion.h"

class TransformComponent : public BaseComponent
{
public:
    Vector3f position;
    Vector3f scale;
	Quaternion orientation;

    TransformComponent();
	TransformComponent(Vector3f pos, Vector3f scale);
	TransformComponent(Vector3f pos, Vector3f scale, Quaternion orient);
    TransformComponent(Vector3f pos, Vector3f scale, float angle, Vector3f axis);
    ~TransformComponent();
};

