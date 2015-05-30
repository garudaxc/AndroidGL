#pragma once

#include "Platfrom.h"

enum ShaderUniform
{
	SU_WORLD = 0,
	SU_VIEW,
	SU_PROJECTION,
	SU_TEX_DIFFUSE,
	SU_TEX_NORMAL,
	SU_COUNT,
};


class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool	LoadFromFile(const char* fileName);

	void	Bind();

	void	SetUnifrom(ShaderUniform u, const float* value);
	void	SetUnifrom(ShaderUniform u, int value);
	
private:

	GLuint program_;
	
	GLint	uniform_[SU_COUNT];

};

extern ShaderManager GShaderManager;
