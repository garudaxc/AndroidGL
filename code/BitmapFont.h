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
	void		SetViewPort(uint32_t viewWidth, uint32_t viewHeight);

	void		DrawString(SpriteBatch* sprite, const char* text, const Vector3f& pos, float scale = 1.f, const Color& color = Color::WHITE);

	void		DrawString3D(SpriteBatch* sprite, const char* text, const Vector3f& pos, const Vector3f& normal,
							const Vector3f& up, float scale, const Color& color, const Matrix4f& matViewProj);

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


	GLuint		texture_;
};
