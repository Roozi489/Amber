#pragma once
#include "BaseComponent.h"
#include "Vector.h"
#include "Quaternion.h"

class TransformComponent : public BaseComponent
{
public:
    Vector3f position;
	Quaternion orientation;
    Vector3f scale;

    TransformComponent();
	TransformComponent(Vector3f pos, Vector3f scale);
	TransformComponent(Vector3f pos, Quaternion orient, Vector3f scale);
    TransformComponent(Vector3f pos, float angle, Vector3f axis, Vector3f scale);
    ~TransformComponent();
};

