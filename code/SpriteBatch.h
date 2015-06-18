#pragma once
#include "Platfrom.h"
#include "Color.h"
#include "AurMath.h"
#include <vector>

using namespace Aurora;
using namespace std;

class SpriteBatch
{
public:
	SpriteBatch();
	~SpriteBatch();

	void	Init(uint32_t maxSprites);
	void	Commit(uint32_t viewWidth, uint32_t viewHeight, const Matrix4f& ViewProj);
	
private:
	

	struct Sprite
	{
		Vector3f	pos;
		float		w;		//size
		float		h;		
		rect_t		uvRect;
		GLuint		texture;
	};

	vector<Sprite>		sprites_;
	uint32_t			spriteCount_;		//current used
};
