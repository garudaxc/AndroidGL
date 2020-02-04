#pragma once

#include "Platfrom.h"
#include "AurMath.h"

using namespace FancyTech;

enum ShaderUniform
{
	SU_WORLD = 0,
	SU_VIEW,
	SU_PROJECTION,
	SU_VIEWPROJ,
	SU_TEX_DIFFUSE,
	SU_TEX_NORMAL,
	SU_COUNT,
};

enum EShader
{
	ShaderDiffuse = 0,
	ShaderUI,
	ShaderBump,
	ShaderParticle,

	ShaderCount
};


enum VertexAttributeLocation
{
	VERTEX_ATTRIBUTE_LOCATION_POSITION = 0,
	VERTEX_ATTRIBUTE_LOCATION_NORMAL = 1,
	VERTEX_ATTRIBUTE_LOCATION_TANGENT = 2,
	VERTEX_ATTRIBUTE_LOCATION_BINORMAL = 3,
	VERTEX_ATTRIBUTE_LOCATION_COLOR = 4,
	VERTEX_ATTRIBUTE_LOCATION_UV0 = 5,
	VERTEX_ATTRIBUTE_LOCATION_UV1 = 6,
	VERTEX_ATTRIBUTE_LOCATION_JOINT_INDICES = 7,
	VERTEX_ATTRIBUTE_LOCATION_JOINT_WEIGHTS = 8,
	VERTEX_ATTRIBUTE_LOCATION_FONT_PARMS = 9
};


class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool	LoadFromFile(EShader s, const char* fileName);

	void	Bind(EShader s);

	void	SetUnifrom(ShaderUniform u, const Matrix4f& matrix);
	void	SetUnifrom(ShaderUniform u, int value);

	void	TestComputeShader();
	void	RunComputerShader(float time, const Matrix4f& matViewProj);
	
private:

	GLuint program_[ShaderCount];
	GLint	uniform_[ShaderCount][SU_COUNT];

	EShader		currentBind_;

};

extern ShaderManager GShaderManager;
