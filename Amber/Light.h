#pragma once
#include "Vector.h"

#include <GL/glew.h>

class Light
{
public:
    Light(float power);
    ~Light();

    GLuint getPositionID() const;
    void setPositionID(GLuint lightLocationID);
    GLuint getDirectionID() const;
    void setDirectionID(GLuint lightDirectionID);
    GLuint getPowerID() const;
    void setPowerID(GLuint powerID);

    Vector3f getPosition() const;
	void setPosition(Vector3f value);
	Vector3f getDirection() const;
	void setDirection(Vector3f value);
    float getPower() const;
    void setPower(float value);

    void toggle();
    bool isOn() const;
private:
    bool mOn;

    GLuint mPositionID;
    GLuint mDirectionID;
    GLuint mPowerID;

	Vector3f mPosition;
	Vector3f mDirection;
    float mPower;
};

