#pragma once
#include "Platfrom.h"

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

	class FrameBuffer;

	class Texture
	{
	public:
		Texture();
		~Texture();

		bool	Load(const char* fileName);

		void	Bind();

		void	Create(int width, int height, TEXTURE_FORMAT fmt);

	private:

		uint32_t	target_;
		GLuint		texId_;
		TEXTURE_FORMAT format_;

		friend class FrameBuffer;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		void	Create(int width, int height, TEXTURE_FORMAT fmt, int depth);
		
		Texture		GetTexture()
		{
			return tex_;
		}

		void	Bind();
		void	Unbind();
	private:
		GLuint		fboId_;
		GLuint		depthRenderbuffer;
		Texture		tex_;
	};
	

	GLuint CreateTextureETC1(const ubyte_t* data, uint32_t w, uint32_t h,
		uint32_t pixelsize, uint32_t stride);

	GLuint LoadPKM(const ubyte_t* fileData, uint32_t size);

}
