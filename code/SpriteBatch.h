#pragma once
#include "Platfrom.h"
#include "Color.h"

using namespace Aurora;

class SpriteBatch
{
public:
	SpriteBatch();
	~SpriteBatch();

	void	Draw(GLuint texture, const rect_t& uvRect, float scale = 1.f, const Color& color = Color::WHITE);

private:
		

};
