#include <algorithm>
#include "BitmapFont.h"
#include "MyLog.h"
#include "FileSystem.h"
#include "ShaderManager.h"
#include "SpriteBatch.h"
#include "glUtil.h"


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
	File* file = GFileSys->OpenFile(fileName);
	if (file ==  NULL){
		return false;
	}

	for (char const* magic = BitmapFontMagic; *magic; magic++) {
		ubyte_t c;
		if (!file->Read<ubyte_t>(c) || c != *magic) {
			GLog.LogError("BitmapFont file invalid! : %s", fileName);
			return false;
		}
	}

	uint32_t glyphCount;
	file->Read(glyphCount);

	glyphs_.resize(glyphCount);
	file->ReadArray(&glyphs_[0], glyphCount);

	file->Read(lineSpacing_);
	file->Read(defaultChar_);

	uint32_t textureFormat, textureStride, textureRows;
	file->Read(textureWidth_);
	file->Read(textureHeight_);
	file->Read(textureFormat);
	file->Read(textureStride);
	file->Read(textureRows);
	
	if (textureFormat != DXGI_FORMAT_R8G8B8A8_UNORM){
		GLog.LogError("BitmapFont texture format error, expect rgba8!");
		return false;
	}

	vector<ubyte_t> texel(textureStride * textureRows);
	file->ReadArray(&texel[0], texel.size());

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth_, textureHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texel[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	file->Close();
	
	return true;
}

bool operator < (const BitmapFont::Glyph& glyph, char c)
{
	return glyph.character < (uint32_t)c;
}

const BitmapFont::Glyph& BitmapFont::FindGlyph(char c) const
{
	auto it = lower_bound(glyphs_.begin(), glyphs_.end(), c);
	if (it != glyphs_.end()){
		return *it;
	}

	if (defaultChar_ != 0){
		it = lower_bound(glyphs_.begin(), glyphs_.end(), defaultChar_);
		return *it;
	}

	return glyphs_.front();
}


void BitmapFont::DrawString(SpriteBatch* sprite, const char* text, const Vector3f& pos, float scale, const Vector4f& color)
{
	const char* c = text;

	float x = pos.x;
	float y = pos.y;

	while (*c){
		if (*c == '\n'){
			y -= lineSpacing_ * scale;
			x = pos.x;
		} else {
			auto glyph = FindGlyph(*c);

			x += glyph.xoffset * scale;

			sprite->Draw(texture_, glyph.subrect, textureWidth_, textureHeight_,
				Vector3f(x, y - glyph.yoffset * scale, 0.f), scale, color);

			x += ((glyph.subrect.right - glyph.subrect.left) + glyph.xAdvance) * scale;
		}

		c++;
	}
}

void BitmapFont::DrawString3D(SpriteBatch* sprite, const char* text, const Vector3f& pos, const Vector3f& normal,
	const Vector3f& up, float scale, const Vector4f& color)
{
	const char* c = text;

	float x = pos.x;
	float y = pos.y;

	Vector3f p = pos;

	Vector3f right = Vector3f::Cross(up, normal);

	Matrix4f mRot(
		right.x, right.y, right.z, 0.f,
		up.x, up.y, up.z, 0.f,
		normal.x, normal.y, normal.z, 0.f,
		0.f, 0.f, 0.f, 1.f);

	Quaternionf qRot = mRot.ToQuaternion();	

	while (*c){
		if (*c == '\n'){
			p -= up * (lineSpacing_ * scale);
			p.x = pos.x;
		} else {

			auto glyph = FindGlyph(*c);
			p += right * glyph.xoffset * scale;
			Vector3f anchor = p - up * (glyph.yoffset * scale);
			
			sprite->Draw3D(texture_, glyph.subrect, textureWidth_, textureHeight_,
								anchor, qRot, scale, color);

			p += right * ((glyph.subrect.right - glyph.subrect.left + glyph.xAdvance) * scale);
		}

		c++;
	}
}


void BitmapFont::BindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);
}

