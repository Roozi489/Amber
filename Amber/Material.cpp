#include "Material.h"


Material::Material()
{
}

void Material::setRoughness(float value)
{
	mRoughness = value;
}

float Material::getRoughness() const
{
	return mRoughness;
}