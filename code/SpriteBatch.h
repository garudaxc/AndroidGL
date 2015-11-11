#pragma once
#include "Platfrom.h"
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

	void	Draw(GLuint texture, const rect_t& uvRect, uint32_t texWidth, uint32_t texHeight,
		const Vector3f& pos, float scale, const Vector4f& color);

	void	Draw3D(GLuint texture, const rect_t& uvRect, uint32_t texWidth, uint32_t texHeight,
		const Vector3f& pos, const Quaternionf& rot,
		float scale, const Vector4f& color);

	void	Commit(uint32_t viewWidth, uint32_t viewHeight, const Matrix4f& ViewProj = Matrix4f::IDENTITY);
	
private:

	void	Draw(GLuint texture, const rect_t& uvRect, uint32_t texWidth, uint32_t texHeight,
		const Vector3f& pos, const Quaternionf& rot,
		float scale, const Vector4f& color, bool is3d);
	
	struct Sprite
	{
		Vector3f	pos;
		uint32_t	color;
		float		uvLeft, uvRight, uvTop, uvBottom;
		Quaternionf	rotation;
		float		w;		//size
		float		h;		
		GLuint		texture;
		bool		is3d;
	};

	vector<Sprite>		sprites_;
	uint32_t			spriteCount_;		//current used

	struct Geometry *	geometry_;
};
