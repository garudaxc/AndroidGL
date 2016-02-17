#pragma once
#include "Platfrom.h"

//GLuint CreateTexture(const uint8_t* data, uint32_t w, uint32_t h, uint32_t stride);

namespace FancyTech
{

	enum TEXTURE_FORMAT
	{
		RGB8,
		RGBA8,
		ETC1,
		ETC2,
		DXT1,
		DXT5,
	};


	GLuint CreateTexture(const ubyte_t* data, uint32_t w, uint32_t h,
		uint32_t pixelsize, uint32_t stride);

	GLuint LoadPKM(const ubyte_t* fileData, uint32_t size);
}
