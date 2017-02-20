#pragma once
#include "Graphics/GL/Texture.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace Amber
{

class TextureManager
{
public:
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

	TextureManager();
	~TextureManager();

	void destroy();

	void preloadTexture(const std::string& fileName);
	Texture* getTexture(const std::string& name);
};

}