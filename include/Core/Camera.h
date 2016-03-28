#pragma once
#include "Core/Time.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

namespace Amber
{
struct CameraDirs
{
	Vector3f dir;
	Vector3f up;
};

class Camera
{
public:
	static const CameraDirs CameraDirections[];

	// TODO implement these
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

	Vector3f forwardVector() const;
	Vector3f upVector() const;
	Vector3f rightVector() const;

	Vector3f getPosition() const;
	void setPosition(Vector3f pos);
	float getFieldOfViewX() const;
	void setFieldOfViewX(float fovX);

	float getNearPlane() const;
	float getFarPlane() const;

	const Matrix4x4f& getProjectionMatrix() const;
	const Matrix4x4f& getViewMatrix() const;

private:
	float m_nearPlane = 0.01f;
	float m_farPlane = 10000.f;
	float m_fieldOfViewX;

	Quaternion m_orientation;
	Vector3f m_position;

	Matrix4x4f m_projectionMatrix;
	Matrix4x4f m_viewMatrix;

	void recomputeViewMatrix();
};

}