#pragma once
#include "Platfrom.h"
#include "Color.h"
#include "AurMath.h"
#include <vector>

using namespace std;
using namespace Aurora;

class SpriteBatch;

class BitmapFont
{
public:
	BitmapFont();
	~BitmapFont();

	bool		LoadFromFile(const char* fileName);

	void		DrawString(SpriteBatch* sprite, const char* text, const Vector3f& pos, float scale = 1.f, const Color& color = Color::WHITE);

	struct Glyph
	{
		uint32_t character;
		rect_t subrect;
		float xoffset;
		float yoffset;
		float xAdvance;
	};

private:
	vector<Glyph>	glyphs_;
	float			lineSpacing_;
	uint32_t		defaultChar_;

	GLuint		texture_;
};
