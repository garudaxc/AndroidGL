#include "BitmapFont.h"
#include "MyLog.h"
#include "FileSystem.h"


BitmapFont::BitmapFont() :texture_(0)
{
}

BitmapFont::~BitmapFont()
{
}


static const char BitmapFontMagic[] = "DXTKfont";


#define DXGI_FORMAT_R8G8B8A8_UNORM		28
#define DXGI_FORMAT_BC2_UNORM			74

bool BitmapFont::LoadFromFile(const char* fileName)
{
	File file;
	if (!file.Open(fileName)){
		return false;
	}

	for (char const* magic = BitmapFontMagic; *magic; magic++) {
		ubyte_t c;
		if (!file.Read<ubyte_t>(c) || c != *magic) {
			GLog.LogError("BitmapFont file invalid! : %s", fileName);
			return false;
		}
	}

	uint32_t glyphCount;
	file.Read(glyphCount);

	glyphs_.resize(glyphCount);
	file.ReadArray(&glyphs_[0], glyphCount);

	file.Read(lineSpacing_);
	file.Read(defaultChar_);

	uint32_t textureWidth, textureHeight, textureFormat, textureStride, textureRows;
	file.Read(textureWidth);
	file.Read(textureHeight);
	file.Read(textureFormat);
	file.Read(textureStride);
	file.Read(textureRows);

	if (textureFormat != DXGI_FORMAT_R8G8B8A8_UNORM){
		GLog.LogError("BitmapFont texture format error, expect rgba8!");
		return false;
	}

	vector<ubyte_t> texel(textureStride * textureRows);

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texel[0]);

	//checkGlError("glTexImage2D");
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Draw left quad with repeat wrap mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}


void BitmapFont::DrawString(SpriteBatch* sprite, const char* text, const Vector3f& pos, float scale, const Color& color)
{

}