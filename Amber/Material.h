#pragma once


// Used for PBR rendering
class Material
{
public:
	Material();

	void setRoughness(float value);
	float getRoughness() const;
private:
	float mRoughness = 0.f;
	float mMetalness = 0.f;
};