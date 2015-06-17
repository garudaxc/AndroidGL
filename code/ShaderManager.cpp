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


ShaderManager::ShaderManager() :currentBind_(ShaderDiffuse)
{
	memset(program_, 0, sizeof(program_));
	memset(uniform_, 0, sizeof(uniform_));
}

ShaderManager::~ShaderManager()
{
}


bool ShaderManager::LoadFromFile(EShader s, const char* fileName)
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

	program_[s] = glCreateProgram();
	glBindAttribLocation(program_[s], VERTEX_ATTRIBUTE_LOCATION_POSITION, "vPosition");
	glBindAttribLocation(program_[s], VERTEX_ATTRIBUTE_LOCATION_NORMAL, "vNormal");
	glBindAttribLocation(program_[s], VERTEX_ATTRIBUTE_LOCATION_COLOR, "vColor");
	glBindAttribLocation(program_[s], VERTEX_ATTRIBUTE_LOCATION_UV0, "vUV");

	glAttachShader(program_[s], vs);
	glAttachShader(program_[s], ps);
	glDeleteShader(vs);
	glDeleteShader(ps);
	glLinkProgram(program_[s]);
	glGetProgramiv(program_[s], GL_LINK_STATUS, &state);
	if (state != GL_TRUE) {
		GLog.LogError("link shader failed!");

		GLint infoLen = 0;
		glGetProgramiv(program_[s], GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 0) {
			vector<char> err(infoLen + 1);
			glGetProgramInfoLog(program_[s], infoLen + 1, NULL, &err[0]);
			GLog.LogError(&err[0]);
		}

		return false;
	}

	GLint loc = -1;
	loc = glGetAttribLocation(program_[s], "vPosition");
	GLog.LogInfo("vPostition attrib location %d", loc);
	loc = glGetAttribLocation(program_[s], "vNormal");
	GLog.LogInfo("vNormal attrib location %d", loc);
	loc = glGetAttribLocation(program_[s], "vColor");
	GLog.LogInfo("vColor attrib location %d", loc);
	loc = glGetAttribLocation(program_[s], "vUV");
	GLog.LogInfo("vUV attrib location %d", loc);

	uniform_[s][SU_WORLD] = glGetUniformLocation(program_[s], "mWorld");
	GLog.LogInfo("uniform_[SU_WORLD] %d", uniform_[s][SU_WORLD]);

	uniform_[s][SU_VIEW] = glGetUniformLocation(program_[s], "mView");
	GLog.LogInfo("uniform_[SU_VIEW] %d", uniform_[s][SU_VIEW]);

	uniform_[s][SU_PROJECTION] = glGetUniformLocation(program_[s], "mProj");
	GLog.LogInfo("uniform_[SU_PROJECTION] %d", uniform_[s][SU_PROJECTION]);

	uniform_[s][SU_TEX_DIFFUSE] = glGetUniformLocation(program_[s], "texDiffuse");
	GLog.LogInfo("uniform_[SU_TEX_DIFFUSE] %d", uniform_[s][SU_TEX_DIFFUSE]);

	uniform_[s][SU_TEX_NORMAL] = glGetUniformLocation(program_[s], "texNormal");
	GLog.LogInfo("uniform_[SU_TEX_NORMAL] %d", uniform_[s][SU_TEX_NORMAL]);

	//GLint unifromSampler = glGetUniformLocation(program, "texture1");
	//GLint unifromSampler2 = glGetUniformLocation(program, "texture2");

	return true;
}


void ShaderManager::Bind(EShader s)
{
	currentBind_ = s;
	glUseProgram(program_[s]);
}

void ShaderManager::SetUnifrom(ShaderUniform u, const float* value)
{
	if (u == SU_WORLD || u == SU_VIEW || u == SU_PROJECTION) {
		if (uniform_[currentBind_][u] != -1) {
			glUniformMatrix4fv(uniform_[currentBind_][u], 1, GL_FALSE, value);
		}
	}
}

void ShaderManager::SetUnifrom(ShaderUniform u, int value)
{
	if (u == SU_TEX_DIFFUSE || u == SU_TEX_NORMAL) {
		if (uniform_[currentBind_][u] != -1) {
			glUniform1i(uniform_[currentBind_][u], value);
		}
	}
}