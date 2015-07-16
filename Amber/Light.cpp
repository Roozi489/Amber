#include "Light.h"

Light::Light(float power)
: mOn(true)
, mPower(power)
, mPosition(Vector3f(0.f))
, mDirection(Vector3f(0.f, 0.f, 1.f))
{

}

Light::~Light()
{
}

GLuint Light::getPositionID() const
{
    return mPositionID;
}

void Light::setPositionID(GLuint lightLocationID)
{
    mPositionID = lightLocationID;
}

GLuint Light::getDirectionID() const
{
    return mDirectionID;
}

void Light::setDirectionID(GLuint lightDirectionID)
{
    mDirectionID = lightDirectionID;
}

GLuint Light::getPowerID() const
{
    return mPowerID;
}

void Light::setPowerID(GLuint powerID)
{
    mPowerID = powerID;
}

Vector3f Light::getPosition() const
{
    return mPosition;
}

void Light::setPosition(Vector3f value)
{
    mPosition = value;
}

Vector3f Light::getDirection() const
{
    return mDirection;
}

void Light::setDirection(Vector3f value)
{
    mDirection = value;
}

float Light::getPower() const
{
    return mPower;
}

void Light::setPower(float value)
{
    mPower = value;
}

void Light::toggle()
{
    mOn = !mOn;
}

bool Light::isOn() const
{
    return mOn;
}