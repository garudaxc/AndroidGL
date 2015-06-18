#include "SpriteBatch.h"
#include "ShaderManager.h"
#include "MyLog.h"
#include "glUtil.h"


struct TextVert
{
	Vector3f	pos;
	Color		color;
	Vector2f	uv;
};



struct Geometry
{
	GLuint		vao_;
	GLuint		vbo_;
	GLuint		ibo_;

	void Init(int numVertex)
	{

#ifdef GL_ARB_vertex_array_object
		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);
#endif

		int numIndex = (numVertex / 2) * 3;

		vector<uint16_t> index(numIndex);
		for (uint16_t i = 0; i < numVertex / 4; i++) {
			index[i * 6 + 0] = i * 4 + 0;
			index[i * 6 + 1] = i * 4 + 3;
			index[i * 6 + 2] = i * 4 + 2;

			index[i * 6 + 3] = i * 4 + 2;
			index[i * 6 + 4] = i * 4 + 1;
			index[i * 6 + 5] = i * 4 + 0;
		}

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TextVert)* numVertex, NULL, GL_DYNAMIC_DRAW);

#ifdef GL_ARB_vertex_array_object
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(TextVert), BUFFER_OFFSET(0));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_COLOR);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(TextVert), BUFFER_OFFSET(12));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV0);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_UV0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVert), BUFFER_OFFSET(28));

#endif

		glGenBuffers(1, &ibo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index[0]) * index.size(), &index[0], GL_STATIC_DRAW);

#ifdef GL_ARB_vertex_array_object
		glBindVertexArray(0);
#endif

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Draw(uint32_t indexCount, uint32_t indexOffset)
	{

#ifdef GL_ARB_vertex_array_object
		glBindVertexArray(vao_);
#else
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(TextVert), BUFFER_OFFSET(0));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_COLOR);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(TextVert), BUFFER_OFFSET(12));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV0);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_UV0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVert), BUFFER_OFFSET(28));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
#endif

		void* index = (void*)(indexOffset * sizeof(unsigned short));
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, index);
	}

};


SpriteBatch::SpriteBatch() :spriteCount_(0), geometry_(NULL)
{
}

SpriteBatch::~SpriteBatch()
{
	if (geometry_ != NULL) {
		delete geometry_;
		geometry_ = NULL;
	}
}

void SpriteBatch::Init(uint32_t maxSprites)
{
	geometry_ = new Geometry();
	geometry_->Init(maxSprites * 4);

	sprites_.resize(maxSprites);
}

void SpriteBatch::Draw(GLuint texture, const rect_t& uvRect, uint32_t texWidth, uint32_t texHeight,
	const Vector3f& pos, const Quaternionf& rot,
	float scale, const Color& color, bool is3d)
{
	Sprite& sprite = sprites_[spriteCount_++];

	float invTexW = 1.f / texWidth;
	float intTexH = 1.f / texHeight;
	
	sprite.color = color;
	sprite.texture = texture;
	sprite.is3d = is3d;
	sprite.rotation = rot;
	
	sprite.pos = pos;
	sprite.w = (float)(uvRect.right - uvRect.left) * scale;
	sprite.h = -(float)(uvRect.bottom - uvRect.top) * scale;		// text render from top down
	sprite.uvLeft = uvRect.left * invTexW;
	sprite.uvRight = uvRect.right * invTexW;
	sprite.uvTop = uvRect.top * intTexH;
	sprite.uvBottom = uvRect.bottom * intTexH;
}

void SpriteBatch::Draw(GLuint texture, const rect_t& uvRect, uint32_t texWidth, uint32_t texHeight,
	const Vector3f& pos, float scale, const Color& color)
{
	Draw(texture, uvRect, texWidth, texHeight, pos, Quaternionf::IDENTITY, scale, color, false);
}


void SpriteBatch::Draw3D(GLuint texture, const rect_t& uvRect, uint32_t texWidth, uint32_t texHeight,
	const Vector3f& pos, const Quaternionf& rot,
	float scale, const Color& color)
{
	Draw(texture, uvRect, texWidth, texHeight, pos, rot, scale, color, true);
}


void SpriteBatch::Commit(uint32_t viewWidth, uint32_t viewHeight, const Matrix4f& ViewProj)
{
	Vector3f invViewPort(2.f / viewWidth, 2.f / viewHeight, 1.f);

	vector<TextVert> vert(sprites_.size());

	TextVert* vertex = &vert[0];
	// 2d text
	for (auto it = sprites_.begin(); it != sprites_.begin() + spriteCount_; ++it) {
		if (it->is3d) {
			continue;
		}

		Vector3f pos = Vector3f::Modulate(it->pos, invViewPort) - Vector3f(1.0f, 1.0f, 0.f);
		float w = it->w * invViewPort.x;
		float h = it->h * invViewPort.y;
		
		vertex->pos = pos;
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvLeft, it->uvTop);
		vertex++;

		vertex->pos = pos + Vector3f(w, 0.f, 0.f);
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvRight, it->uvTop);
		vertex++;

		vertex->pos = pos + Vector3f(w, h, 0.f);
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvRight, it->uvBottom);
		vertex++;

		vertex->pos = pos + Vector3f(0.f, h, 0.f);
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvLeft, it->uvBottom);
		vertex++;
	}
	int vertex2d = vertex - &vert[0];

	// 3d text
	for (auto it = sprites_.begin(); it != sprites_.begin() + spriteCount_; ++it) {
		if (!it->is3d) {
			continue;
		}

		Vector3f pos = it->pos;

		Vector3f right = Vector3f::UNIT_X * it->rotation;
		Vector3f up = Vector3f::UNIT_Y * it->rotation;

		float w = it->w * invViewPort.x;
		float h = it->h * invViewPort.y;

		vertex->pos = pos;
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvLeft, it->uvTop);
		vertex++;

		vertex->pos = pos + right * it->w;
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvRight, it->uvTop);
		vertex++;

		vertex->pos = pos + right * it->w + up * it->h;
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvRight, it->uvBottom);
		vertex++;

		vertex->pos = pos + up * it->h;
		vertex->color = it->color;
		vertex->uv = Vector2f(it->uvLeft, it->uvBottom);
		vertex++;
	}

	int vertex3d = spriteCount_ * 4 - vertex2d;

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, spriteCount_ * 4 * sizeof(TextVert), (void *)&vert[0]);

	GShaderManager.Bind(ShaderUI);
	GShaderManager.SetUnifrom(SU_TEX_DIFFUSE, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprites_.front().texture);

	GShaderManager.SetUnifrom(SU_VIEWPROJ, Matrix4f::IDENTITY);
	geometry_->Draw(vertex2d / 2 * 3, 0);

	GShaderManager.SetUnifrom(SU_VIEWPROJ, ViewProj);
	geometry_->Draw(vertex3d / 2 * 3, vertex2d / 2 * 3);

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	spriteCount_ = 0;
}