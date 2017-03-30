#include "thirdParty/etc1/etc1.h"
#include "TextureFormat.h"
#include "MyLog.h"
#include "glUtil.h"

namespace FancyTech
{

	GLuint CreateTexture(const ubyte_t* data, uint32_t w, uint32_t h,
		uint32_t pixelsize, uint32_t stride)
	{
		if (data == NULL || w == 0 || h == 0) {
			GLog.LogError("CreateTexture invalid parameter data %p w %d h %d", data, w, h);
			return 0;
		}

		uint32_t size = etc1_get_encoded_data_size(w, h);
		GLog.LogInfo("etc1_get_encoded_data_size %u", size);

		ubyte_t* buffer = (ubyte_t*)malloc(size);

		if (buffer == NULL)	{
			GLog.LogError("malloc failed size %u", size);
			return 0;
		}
		uint32_t time = GetTicksMS();
		if (etc1_encode_image(data, w, h, pixelsize, stride, buffer) != 0) {
			GLog.LogError("etc1_encode_image failed!");
			free(buffer);
			return 0;
		}
		GLog.LogInfo("etc1 encode time %u ms", GetTicksMS() - time);

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		uint32_t internalfmt = GL_ETC1_RGB8_OES;

		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalfmt, w, h, 0, size, buffer);

		checkGlError("glCompressedTexImage2D");

		//glGenerateMipmap(GL_TEXTURE_2D);

		// Draw left quad with repeat wrap mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		free(buffer);

		return tex;
	}


	GLuint LoadPKM(const ubyte_t* fileData, uint32_t size)
	{
		if (!etc1_pkm_is_valid(fileData)) {
			GLog.LogError("pkm file header invalid!");
			return 0;
		}
		
		uint32_t width	= etc1_pkm_get_width(fileData);
		uint32_t height = etc1_pkm_get_height(fileData);

		GLog.LogInfo("pkm file width %d height %d", width, height);

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		uint32_t internalfmt = GL_ETC1_RGB8_OES;

		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalfmt, width, height, 
							0, size - ETC_PKM_HEADER_SIZE, 
							fileData + ETC_PKM_HEADER_SIZE);

		checkGlError("glCompressedTexImage2D");

		//glGenerateMipmap(GL_TEXTURE_2D);

		// Draw left quad with repeat wrap mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		return tex;
	}


}