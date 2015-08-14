#include "Camera.h"
#include "Globals.h"
#include "Vector.h"
#include "Matrix.h"

#include <cmath>

Camera::Camera()
{

}

void Camera::init()
{
    position = Vector3f::Zero;
	orientation = Quaternion::Identity;
    viewTarget = position + Vector3f::Forward;

	// TODO: check whether the fov should't be in radians
	projectionMatrix = Matrix4x4f::perspectiveFov(50.f, static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight), nearPlane, farPlane);
}

void Camera::update(float delta)
{
	// TODO: move to postionTarget
	// 
}

void Camera::offsetOrientation(float yaw, float pitch)
{
	// absolute up
	const Quaternion yawRot = angleAxis(yaw, Vector3f::Up);
	// relative right
	const Quaternion pitchRot = angleAxis(pitch, orientation * Vector3f::Right);

	orientation = yawRot * pitchRot * orientation;
}

void Camera::moveForward(float value)
{
	position += orientation * Vector3f::Forward * value;
}

void Camera::moveBackward(float value)
{
	position += orientation * Vector3f::Backward * value;
}

void Camera::moveLeft(float value)
{
	position += orientation * Vector3f::Left * value;
}

void Camera::moveRight(float value)
{
	position += orientation * Vector3f::Right * value;
}

void Camera::moveUp(float value)
{
	position += orientation * Vector3f::Up * value;
}

void Camera::moveDown(float value)
{
	position += orientation * Vector3f::Down * value;
}

void Camera::rotateUp(float value)
{
	offsetOrientation(0.f, value);
}

void Camera::rotateDown(float value)
{
	offsetOrientation(0.f, -value);
}

void Camera::rotateLeft(float value)
{
	offsetOrientation(value, 0.f);
}

void Camera::rotateRight(float value)
{
	offsetOrientation(-value, 0.f);
}

Vector3f Camera::getPosition() const
{
    return position;
}

void Camera::setPosition(Vector3f pos)
{
	position = pos;
}

Vector3f Camera::getViewTarget() const
{
    return viewTarget;
}

void Camera::setViewTarget(Vector3f t)
{
    viewTarget = t;
}

const Matrix4x4f& Camera::getProjectionMatrix()
{
    return projectionMatrix;
}
