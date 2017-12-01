#pragma once
#include <string>
#include "..\dependencies\stb\stb_image.h"

class MyTexture
{
public:
	MyTexture();
	MyTexture(const std::string& fileName, unsigned int location, unsigned int textureSpot, unsigned int filterType, bool useAlpha = false);
	
	unsigned int GetTexture() const;
	unsigned int GetLocation() const;
	unsigned int GetSpot() const;

	virtual ~MyTexture();
private:
	unsigned int m_texture;
	unsigned int m_location;
	unsigned int m_textureSpot;
};