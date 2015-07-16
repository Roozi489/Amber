#pragma once
#include "Utility.h"
#include "Vector.h"
#include "Matrix.h"

class Camera
{
public:
    Camera();

	void init();

    void update(float delta);

    void move(float x, float y, float z);
    void moveForward(float value);
    void moveSideways(float value);
    void rotate(float x, float y, float z);

    Vector3f getPosition() const;
    void setPosition(Vector3f pos);
    void setPosition(float x, float y, float z);
    Vector3f getTarget() const;
    void setTarget(Vector3f pos);
    void setTarget(float x, float y, float z);
	float getRotationY();
    void setMoveDirection(float x, float y, float z);
    void setMoveDirectionX(float value);
    void setMoveDirectionY(float value);
    void setMoveDirectionZ(float value);

	// PERF: should return pointer
	Matrix4x4f getProjectionMatrix();
private:
    Vector3f mPosition;
    Vector3f mTarget;
    Vector3f mMoveDirection;

	Matrix4x4f mProjectionMatrix;
};

