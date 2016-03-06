#include "Core/Camera.h"
#include "Core/Globals.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Window/Input.h"
#include "Math/Math.h"

namespace Amber
{

Camera::Camera()
{

}

void Camera::init()
{
	position = Vector3f::Zero;
	orientation = Quaternion::Identity;

	// TODO: check whether the fov should't be in radians
	m_projectionMatrix = perspectiveFov(toRadians(60.f), static_cast<float>(g_window.getWidth()), static_cast<float>(g_window.getHeight()), m_nearPlane);
	m_viewMatrix = quaternionToMatrix4x4f(conjugate(g_camera.orientation)) * Matrix4x4f::translate(-g_camera.position);
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
}

void Camera::offsetOrientation(float yaw, float pitch)
{
	// absolute up
	const Quaternion yawRot = angleAxis(yaw, Vector3f::Up);
	// relative right
	const Quaternion pitchRot = angleAxis(pitch, orientation * Vector3f::Right);

	orientation = yawRot * pitchRot * orientation;
	recomputeViewMatrix();
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
	recomputeViewMatrix();
}

const Matrix4x4f& Camera::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

const Matrix4x4f& Camera::getViewMatrix() const
{
	return m_viewMatrix;
}

Matrix4x4f Camera::perspectiveFov(float fovX, float width, float height, float zNear)
{
	float ratio = (height / width);
	auto e = 1.f / tan(fovX * ratio / 2);
	Matrix4x4f result(0.f);
	result[0][0] = e;
	result[1][1] = e / ratio;
	result[2][2] = -1;
	result[2][3] = -1;
	result[3][2] = -2 * zNear;
	return result;
}

Matrix4x4f Camera::lookAt(Vector3f& eye, Vector3f center, Vector3f up)
{
	Vector3f f(normalize(center - eye));
	Vector3f s(normalize(cross(f, up)));
	Vector3f u(cross(s, f));

	Matrix4x4f result(0);
	result.fillDiag(1);

	result[0][0] = s.x;
	result[1][0] = s.y;
	result[2][0] = s.z;
	result[0][1] = u.x;
	result[1][1] = u.y;
	result[2][1] = u.z;
	result[0][2] = -f.x;
	result[1][2] = -f.y;
	result[2][2] = -f.z;
	result[3][0] = -dot(s, eye);
	result[3][1] = -dot(u, eye);
	result[3][2] = dot(f, eye);
	return result;
}

void Camera::recomputeViewMatrix()
{
	m_viewMatrix = quaternionToMatrix4x4f(conjugate(g_camera.orientation)) * Matrix4x4f::translate(-g_camera.position);
}

}
