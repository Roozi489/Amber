#pragma once
#include "Texture.h"

#include <string>
#include <unordered_map>
#include <memory>

class TextureManager
{
public:
	std::unordered_map<std::string, Texture> textures;
	
	TextureManager();
	~TextureManager();

	bool preloadTexture(const std::string& fileName);
	Texture* getTexture(const std::string& name);
};

