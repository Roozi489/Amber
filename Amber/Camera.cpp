#include "Camera.h"
#include "Globals.h"
#include "Vector.h"
#include "Matrix.h"

#include <GL/glew.h>

#include <cmath>

Camera::Camera()
{

}

void Camera::init()
{
    mPosition = Vector3f(0.f);
    mTarget = mPosition + Vector3f(0.f, 0.f, 0.1f);

    mProjectionMatrix = Matrix4x4f::perspectiveFov(50.f, (float)gWindowWidth, (float)gWindowHeight, 0.1f, 10000.f);
}


void Camera::update(float delta)
{
    moveForward(mMoveDirection.z * (delta));
    moveSideways(mMoveDirection.x * (delta));
}

void Camera::move(float x, float y, float z)
{
    mPosition.x += x;
    mPosition.y += y;
    mPosition.z += z;
}

void Camera::moveForward(float value)
{
    Vector3f distance = mTarget - mPosition;
    mPosition = mPosition + (distance * value);
    mTarget = mTarget + (distance * value);
}

void Camera::moveSideways(float value)
{
    Vector3f distance = mTarget - mPosition;
    Vector2f tmp(distance.x, distance.z);
    Vector2f tmpNormal(tmp.y, -tmp.x);
    distance.x = tmpNormal.x;
    distance.y = 0.f;
    distance.z = tmpNormal.y;
    mPosition = mPosition + distance * value;
    mTarget = mTarget + (distance * value);
}


void Camera::rotate(float x, float y, float z)
{
    Vector3f distance = mTarget - mPosition;
    float curAngleX = toDegrees(std::atan2f(distance.z, distance.y));
    float curAngleY = toDegrees(std::atan2f(distance.z, distance.x));

    Vector3f newDistance;
    newDistance.z = std::sinf(toRadians(curAngleY + y)) * 0.1f;
    newDistance.x = std::cosf(toRadians(curAngleY + y)) * 0.1f;
    float tmpY = mTarget.y;
    mTarget = mPosition + newDistance;
    mTarget.y = tmpY;// -x / 10000.f;

    mTarget.toString();
}

Vector3f Camera::getPosition() const
{
    return mPosition;
}

void Camera::setPosition(Vector3f pos)
{
    setPosition(pos.x, pos.y, pos.z);
}

void Camera::setPosition(float x, float y, float z)
{
    Vector3f distance = mTarget - mPosition;
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;
    mTarget = mPosition + distance;
}

Vector3f Camera::getTarget() const
{
    return mTarget;
}

void Camera::setTarget(Vector3f pos)
{
    mTarget = pos;
}

void Camera::setTarget(float x, float y, float z)
{
    mTarget.x = x;
    mTarget.y = y;
    mTarget.z = z;
}

float Camera::getRotationY()
{
    Vector3f distance = mTarget - mPosition;
    return std::atan2f(distance.z, distance.x);
}

void Camera::setMoveDirection(float x, float y, float z)
{
    mMoveDirection.x = x;
    mMoveDirection.y = y;
    mMoveDirection.z = z;
}

void Camera::setMoveDirectionX(float value)
{
    mMoveDirection.x = value;
}

void Camera::setMoveDirectionY(float value)
{
    mMoveDirection.y = value;
}

void Camera::setMoveDirectionZ(float value)
{
    mMoveDirection.z = value;
}

Matrix4x4f Camera::getProjectionMatrix()
{
    return mProjectionMatrix;
}
