#pragma once
#include "Core/Time.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

namespace Amber
{

class Camera
{
public:
	bool lockOnTarget = false;
	bool smoothMovement = false;

	// TODO: change to TranformComponent* ???
	//Vector3f viewTarget;

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

	const Matrix4x4f& getProjectionMatrix() const;
	const Matrix4x4f& getViewMatrix() const;
	
	static Matrix4x4f perspectiveFov(float fov, float width, float height, float zNear);
	static Matrix4x4f lookAt(Vector3f& eye, Vector3f center, Vector3f up);

private:
	float m_nearPlane = 0.1f;
	float m_farPlane = 10000.f;

	Quaternion orientation;
	Vector3f position;

	Matrix4x4f m_projectionMatrix;
	Matrix4x4f m_viewMatrix;

	void recomputeViewMatrix();
};

}