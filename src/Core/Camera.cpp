#include "Core/Camera.h"
#include "Core/Globals.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Window/Input.h"
#include "Math/Math.h"

namespace Amber
{

const CameraDirs Camera::CameraDirections[] =
{
	{ { 1.f, 0.f, 0.f }, { 0.f, -1.f, 0.f } },
	{ { -1.f, 0.f, 0.f }, { 0.f, -1.f, 0.f } },
	{ { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } },
	{ { 0.f, -1.f, 0.f }, { 0.f, 0.f, -1.f } },
	{ { 0.f, 0.f, 1.f }, { 0.f, -1.f, 0.f } },
	{ { 0.f, 0.f, -1.f }, { 0.f, -1.f, 0.f } },
};

Camera::Camera()
{

}

void Camera::init()
{
	m_position = Vector3f::Zero;
	m_orientation = Quaternion::Identity;
	m_fieldOfViewX = toRadians(60.f);

	m_projectionMatrix = Math::perspectiveFov(m_fieldOfViewX, g_window.windowRatio(), m_nearPlane, m_farPlane);
	m_viewMatrix = quaternionToMatrix4x4f(conjugate(g_camera.m_orientation)) * Math::translate(-g_camera.m_position);
}

void Camera::update(Time delta)
{
	recomputeViewMatrix();

	if (Input::isKeyDown(SDL_SCANCODE_W))
		moveForward();
	if (Input::isKeyDown(SDL_SCANCODE_S))
		moveBackward();
	if (Input::isKeyDown(SDL_SCANCODE_A))
		moveLeft();
	if (Input::isKeyDown(SDL_SCANCODE_D))
		moveRight();
	if (Input::isKeyDown(SDL_SCANCODE_I))
		moveUp();
	if (Input::isKeyDown(SDL_SCANCODE_K))
		moveDown();

	if (Input::MouseRightDown())
	{
		g_window.showCursor(false);
		g_camera.offsetOrientation(Input::mouseRelativeChangeX() / 500.f, Input::mouseRelativeChangeY() / 500.f);
		g_window.setCursorPosition(g_window.getWidth() / 2, g_window.getHeight() / 2);
	}
	else
	{
		g_window.showCursor(true);
	}
}

void Camera::offsetOrientation(float yaw, float pitch)
{
	// absolute up
	const Quaternion yawRot = angleAxis(yaw, Vector3f::Up);
	// relative right
	const Quaternion pitchRot = angleAxis(pitch, m_orientation * Vector3f::Right);

	m_orientation = yawRot * pitchRot * m_orientation;
	recomputeViewMatrix();
}

void Camera::moveForward(float value)
{
	m_position += forwardVector() * value;
}

void Camera::moveBackward(float value)
{
	m_position += -forwardVector() * value;
}

void Camera::moveLeft(float value)
{
	m_position += -rightVector() * value;
}

void Camera::moveRight(float value)
{
	m_position += rightVector()  * value;
}

void Camera::moveUp(float value)
{
	m_position += upVector() * value;
}

void Camera::moveDown(float value)
{
	m_position += -upVector() * value;
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

Vector3f Camera::forwardVector() const
{
	return m_orientation * Vector3f::Forward;
}

Vector3f Camera::upVector() const
{
	return m_orientation * Vector3f::Up;
}

Vector3f Camera::rightVector() const
{
	return m_orientation * Vector3f::Right;
}

Vector3f Camera::getPosition() const
{
	return m_position;
}

void Camera::setPosition(Vector3f pos)
{
	m_position = pos;
	recomputeViewMatrix();
}

float Camera::getFieldOfViewX() const
{
	return m_fieldOfViewX;
}

void Camera::setFieldOfViewX(float fovX)
{
	m_fieldOfViewX = fovX;
}

const Matrix4x4f& Camera::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

const Matrix4x4f& Camera::getViewMatrix() const
{
	return m_viewMatrix;
}

float Camera::getNearPlane() const
{
	return m_nearPlane;
}

float Camera::getFarPlane() const
{
	return m_farPlane;
}

void Camera::recomputeViewMatrix()
{
	m_viewMatrix = quaternionToMatrix4x4f(conjugate(g_camera.m_orientation)) * Math::translate(-g_camera.m_position);
}

}
