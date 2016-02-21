#include "TextureManager.h"



TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

void TextureManager::destroy()
{
	for (auto& pair : textures)
		pair.second->destroy();
}

void TextureManager::preloadTexture(const std::string& fileName)
{
	// Check if it's already loaded
	if (textures.find(fileName) != textures.end())
		return;

	auto texture = std::make_unique<Texture>();

	texture->load(fileName);
	textures.insert(std::make_pair(fileName, std::move(texture)));
}

Texture* TextureManager::getTexture(const std::string& name)
{
	if (textures.find(name) == textures.end())
	{
		preloadTexture(name);
	}

	return textures[name].get();
}