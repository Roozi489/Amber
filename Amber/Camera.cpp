#include "Camera.h"
#include "Globals.h"
#include "Vector.h"
#include "Matrix.h"

Camera::Camera()
{

}

void Camera::init()
{
    position = Vector3f::Zero;
	orientation = Quaternion::Identity;

	// TODO: check whether the fov should't be in radians
	m_projectionMatrix = Matrix4x4f::perspectiveFov(50.f, static_cast<float>(g_WindowWidth), static_cast<float>(g_WindowHeight), m_nearPlane, m_farPlane);
	m_viewMatrix = quaternionToMatrix4x4f(conjugate(g_Camera.orientation)) * Matrix4x4f::translate(-g_Camera.position);
}

void Camera::update(Time delta)
{
	// TODO: add scale
	m_viewMatrix = quaternionToMatrix4x4f(conjugate(g_Camera.orientation)) * Matrix4x4f::translate(-g_Camera.position);
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

const Matrix4x4f& Camera::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

const Matrix4x4f& Camera::getViewMatrix() const
{
	return m_viewMatrix;
}
