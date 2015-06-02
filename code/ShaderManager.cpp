#include "ShaderManager.h"
#include <vector>
#include "MyLog.h"
#include "glUtil.h"

using namespace std;


ShaderManager GShaderManager;


static bool LoadFile(vector<char>& code, const char* fileName)
{
	FILE* pf = fopen(fileName, "r");
	if (pf == NULL) {
		GLog.LogError("open file failed! %s", fileName);
		return false;
	}

	fseek(pf, 0, SEEK_END);
	size_t sz = ftell(pf);
	fseek(pf, 0, SEEK_SET);

	code.resize(sz + 1);
	memset(&code[0], 0, code.size());
	fread(&code[0], sz, 1, pf);

	fclose(pf);

	return true;
}

static bool CompileShader(GLuint shader, const char* include, const char* code)
{
	GLint state = 0;

	const char* src[] = { include, code };

	glShaderSource(shader, 2, src, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
	if (state != GL_TRUE) {
		GLog.LogError("compile shader failed!");

		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 0) {
			vector<char> err(infoLen + 1);
			glGetShaderInfoLog(shader, infoLen + 1, NULL, &err[0]);
			GLog.LogError(&err[0]);
		}

		return false;
	}

	return true;
}



ShaderManager::ShaderManager() :program_(0)
{
}

ShaderManager::~ShaderManager()
{
}


bool ShaderManager::LoadFromFile(const char* fileName)
{
	GLint state = 0;
	bool bRes = false;
	vector<char> code;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	bRes = LoadFile(code, fileName);
	if (!bRes) {
		return false;
	}

	bRes = CompileShader(vs, vsInclude, &code[0]);
	if (!bRes) {
		return false;
	}

	GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
	bRes = LoadFile(code, fileName);
	if (!bRes) {
		return false;
	}
	bRes = CompileShader(ps, psInclude, &code[0]);
	if (!bRes) {
		return false;
	}

	program_ = glCreateProgram();
	glBindAttribLocation(program_, 0, "vPosition");
	glBindAttribLocation(program_, 1, "vNormal");
	//glBindAttribLocation(program_, 2, "vColor");
	glBindAttribLocation(program_, 2, "vUV");

	glAttachShader(program_, vs);
	glAttachShader(program_, ps);
	glDeleteShader(vs);
	glDeleteShader(ps);
	glLinkProgram(program_);
	glGetProgramiv(program_, GL_LINK_STATUS, &state);
	if (state != GL_TRUE) {
		GLog.LogError("link shader failed!");

		GLint infoLen = 0;
		glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 0) {
			vector<char> err(infoLen + 1);
			glGetProgramInfoLog(program_, infoLen + 1, NULL, &err[0]);
			GLog.LogError(&err[0]);
		}

		return false;
	}

	GLint loc = -1;
	loc = glGetAttribLocation(program_, "vPosition");
	GLog.LogInfo("vPostition attrib location %d", loc);
	loc = glGetAttribLocation(program_, "vNormal");
	GLog.LogInfo("vNormal attrib location %d", loc);
	loc = glGetAttribLocation(program_, "vColor");
	GLog.LogInfo("vColor attrib location %d", loc);
	loc = glGetAttribLocation(program_, "vUV");
	GLog.LogInfo("vUV attrib location %d", loc);

	uniform_[SU_WORLD] = glGetUniformLocation(program_, "mWorld");
	GLog.LogInfo("uniform_[SU_WORLD] %d", uniform_[SU_WORLD]);

	uniform_[SU_VIEW] = glGetUniformLocation(program_, "mView");
	GLog.LogInfo("uniform_[SU_VIEW] %d", uniform_[SU_VIEW]);

	uniform_[SU_PROJECTION] = glGetUniformLocation(program_, "mProj");
	GLog.LogInfo("uniform_[SU_PROJECTION] %d", uniform_[SU_PROJECTION]);

	uniform_[SU_TEX_DIFFUSE] = glGetUniformLocation(program_, "texDiffuse");
	GLog.LogInfo("uniform_[SU_TEX_DIFFUSE] %d", uniform_[SU_TEX_DIFFUSE]);

	uniform_[SU_TEX_NORMAL] = glGetUniformLocation(program_, "texNormal");
	GLog.LogInfo("uniform_[SU_TEX_NORMAL] %d", uniform_[SU_TEX_NORMAL]);

	//GLint unifromSampler = glGetUniformLocation(program, "texture1");
	//GLint unifromSampler2 = glGetUniformLocation(program, "texture2");

	return true;
}


void ShaderManager::Bind()
{
	glUseProgram(program_);
}

void ShaderManager::SetUnifrom(ShaderUniform u, const float* value)
{
	if (u == SU_WORLD || u == SU_VIEW || u == SU_PROJECTION) {
		if (uniform_[u] != -1) {
			glUniformMatrix4fv(uniform_[u], 1, GL_FALSE, value);
		}
	}
}

void ShaderManager::SetUnifrom(ShaderUniform u, int value)
{
	if (u == SU_TEX_DIFFUSE || u == SU_TEX_NORMAL) {
		if (uniform_[u] != -1) {
			glUniform1i(uniform_[u], value);
		}
	}
}