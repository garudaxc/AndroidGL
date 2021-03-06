#pragma once
#include "Platfrom.h"
#include "AurMath.h"
#include <vector>

using namespace std;
using namespace FancyTech;

class SpriteBatch;

class BitmapFont
{
public:
	BitmapFont();
	~BitmapFont();

	bool		LoadFromFile(const char* fileName);

	void		DrawString(SpriteBatch* sprite, const char* text, const Vector3f& pos, float scale = 1.f, const Vector4f& color = Vector4f::WHITE);

	void		DrawString3D(SpriteBatch* sprite, const char* text, const Vector3f& pos, const Vector3f& normal,
		const Vector3f& up, float scale, const Vector4f& color);

	void		BindTexture();

	struct Glyph
	{
		uint32_t character;
		rect_t subrect;
		float xoffset;
		float yoffset;
		float xAdvance;
	};

private:

	const Glyph&	FindGlyph(char c) const;

	vector<Glyph>	glyphs_;
	float			lineSpacing_;
	uint32_t		defaultChar_;
	uint32_t		textureWidth_;
	uint32_t		textureHeight_;

	GLuint		texture_;
};
