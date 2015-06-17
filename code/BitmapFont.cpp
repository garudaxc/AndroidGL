#include <algorithm>
#include "BitmapFont.h"
#include "MyLog.h"
#include "FileSystem.h"
#include "ShaderManager.h"
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

uint32_t		textureWidth_;
uint32_t		textureHeight_;
uint32_t		viewWidth_;
uint32_t		viewHeight_;



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

	void Init()
	{

#ifdef GL_ARB_vertex_array_object
		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);
#endif
		
		int numVertex = 1024;
		int numIndex = (numVertex / 2) * 3;

		vector<uint16_t> index(numIndex);
		for (uint16_t i = 0; i < numVertex / 4; i++) {
			index[i * 6 + 0] = i * 4 + 0;
			index[i * 6 + 1] = i * 4 + 1;
			index[i * 6 + 2] = i * 4 + 2;

			index[i * 6 + 3] = i * 4 + 2;
			index[i * 6 + 4] = i * 4 + 3;
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
};

Geometry geometry_;


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

	textureWidth_	= textureWidth;
	textureHeight_	= textureHeight;

	if (textureFormat != DXGI_FORMAT_R8G8B8A8_UNORM){
		GLog.LogError("BitmapFont texture format error, expect rgba8!");
		return false;
	}

	vector<ubyte_t> texel(textureStride * textureRows);
	file.ReadArray(&texel[0], texel.size());

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texel[0]);

	//checkGlError("glTexImage2D");
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Draw left quad with repeat wrap mode

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	geometry_.Init();
	
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


void BitmapFont::SetViewPort(uint32_t viewWidth, uint32_t viewHeight)
{
	viewWidth_ = viewWidth;
	viewHeight_ = viewHeight;
}



void Draw(GLuint texture, rect_t uvRect, const Vector3f& pos, const Color& color)
{
	Vector2f invTex(1.f / textureWidth_, 1.f / textureHeight_);
	Vector3f invViewPort(2.f / viewWidth_, 2.f / viewHeight_, 1.f);

	TextVert vert[4];

	uvRect.right += 100;
	uvRect.bottom += 100;

	Vector3f p = Vector3f::Modulate(pos, invViewPort) - Vector3f(1.0f, 1.0f, 0.f);
	Vector3f size = Vector3f::Modulate(Vector3f(
		(float)(uvRect.right - uvRect.left), 
		(float)(uvRect.bottom - uvRect.top), 0.f), invViewPort) * 5.0f;
	
	vert[0].pos = Vector3f(p.x, p.y, p.z);;
	vert[0].color = color;
	vert[0].uv = Vector2f(uvRect.left * invTex.x, uvRect.top * invTex.y);

	vert[1].pos = Vector3f(p.x + size.x, p.y, p.z);
	vert[1].color = color;
	vert[1].uv = Vector2f(uvRect.right * invTex.x, uvRect.top * invTex.y);

	vert[2].pos = Vector3f(p.x + size.x, p.y + size.y, p.z);
	vert[2].color = color;
	vert[2].uv = Vector2f(uvRect.right * invTex.x, uvRect.bottom * invTex.y);

	vert[3].pos = Vector3f(p.x, p.y + size.y, p.z);
	vert[3].color = color;
	vert[3].uv = Vector2f(uvRect.left * invTex.x, uvRect.bottom * invTex.y);

	glBindVertexArray(geometry_.vao_);
	glBindBuffer(GL_ARRAY_BUFFER, geometry_.vbo_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(TextVert), (void *)&vert[0]);

	GShaderManager.Bind(ShaderUI);
	GShaderManager.SetUnifrom(SU_TEX_DIFFUSE, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, 0);
}

void BitmapFont::DrawString(SpriteBatch* sprite, const char* text, const Vector3f& pos, float scale, const Color& color)
{
	const char* c = text;

	float x = pos.x;
	float y = pos.y;

	while (*c){
		auto glyph = FindGlyph(*c);

		x += glyph.xoffset;

		Draw(texture_, glyph.subrect, Vector3f(x, y + glyph.yoffset, 0.f), color);

		x += (glyph.subrect.right - glyph.subrect.left) + glyph.xAdvance;

		c++;
	}
}