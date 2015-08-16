#include "TextureManager.h"



TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

bool TextureManager::preloadTexture(const std::string& fileName)
{
	// Check if it's already loaded
	if (textures.find(fileName) != textures.end())
		return true;

	auto texture = std::make_unique<Texture>();

	if (texture->load(fileName))
	{
		textures.insert(std::make_pair(fileName, std::move(texture)));
		return true;
	}

	return false;
}

Texture* TextureManager::getTexture(const std::string& name)
{
	if (textures.find(name) == textures.end())
	{
		preloadTexture(name);
	}

	return textures[name].get();
}