#include "Texture.h"
#include "MyLog.h"
#include "glUtil.h"
#include "thirdParty/stb/stb_image.h"




Texture::Texture() :target_(0), texId_(0)
{
}

Texture::~Texture()
{
	if (texId_ != 0){
		glDeleteTextures(1, &texId_);
		texId_ = 0;
	}
}

bool Texture::Load(const char* fileName)
{
	int w, h, comp;
	stbi_uc * t = stbi_load(fileName, &w, &h, &comp, 0);
	if (t == NULL){
		GLog.LogError("Load texture %s failed! : %s", fileName, stbi_failure_reason());
		return false;
	}

	if (comp < 3){
		GLog.LogError("texture %s format not support!", fileName);
		return false;
	}

	target_ = GL_TEXTURE_2D;
	int format = 0;
	if (comp == 3){
		format = GL_RGB;
	} else if (comp == 4){
		format = GL_RGBA;
	}
		
	glGenTextures(1, &texId_);
	glBindTexture(target_, texId_);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(target_, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, t);
	checkGlError("glTexImage2D");
	glGenerateMipmap(GL_TEXTURE_2D);

	// Draw left quad with repeat wrap mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(target_, 0);
	stbi_image_free(t);

	return true;
}

void Texture::Bind()
{
	if (texId_ != 0){
		glBindTexture(target_, texId_);
	}
}
