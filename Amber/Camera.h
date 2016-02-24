#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Time.h"

class Camera
{
public:
	bool lockOnTarget = false;
	bool smoothMovement = false;

	Quaternion orientation;
	Vector3f position;
	Vector3f positionTarget;
	Vector3f viewTarget;

    Camera();

	void init();

    void update(Time delta);

	void offsetOrientation(float yaw, float pitch);

	void moveForward(float value = 1.f);
	void moveBackward(float value = 1.f);
	void moveLeft(float value = 1.f);
	void moveRight(float value = 1.f);
	void moveUp(float value = 1.f);
	void moveDown(float value = 1.f);

	void rotateUp(float value = 0.005f);
	void rotateDown(float value = 0.005f);
	void rotateLeft(float value = 0.005f);
	void rotateRight(float value = 0.005f);

    Vector3f getPosition() const;
    void setPosition(Vector3f pos);
    Vector3f getViewTarget() const;
    void setViewTarget(Vector3f target);

	const Matrix4x4f& getProjectionMatrix();
	const Matrix4x4f& getViewMatrix();
private:
	float nearPlane = 0.1f;
	float farPlane = 10000.f;	

	Matrix4x4f projectionMatrix;
	Matrix4x4f viewMatrix;
};

