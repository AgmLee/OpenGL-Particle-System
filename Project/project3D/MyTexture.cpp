#include "MyTexture.h"
#include <iostream>
#include <gl_core_4_4.h>
//#define STB_IMGAE_IMPLEMENTATION
#include "..\dependencies\stb\stb_image.h"

MyTexture::MyTexture()
{
	m_texture = m_location = m_textureSpot = 0;
}

MyTexture::MyTexture(const std::string& fileName, unsigned int location, unsigned int textureSpot, unsigned int filterType, bool useAlpha)
{
	int width, height, numComponents;
	unsigned char* data;
	if (useAlpha)		
		data = stbi_load((fileName).c_str(), &width, &height, &numComponents, STBI_rgb_alpha);
	else
		data = stbi_load((fileName).c_str(), &width, &height, &numComponents, STBI_rgb);

	if (data == NULL)
		std::cerr << "Unable to load texture: " << fileName << std::endl;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	if (useAlpha)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_textureSpot = textureSpot;
	m_location = location;
}

unsigned int MyTexture::GetTexture() const
{
	return m_texture;
}

unsigned int MyTexture::GetLocation() const
{
	return m_location;
}

unsigned int MyTexture::GetSpot() const
{
	return m_textureSpot;
}

MyTexture::~MyTexture()
{
	glDeleteTextures(1, &m_texture);
}
