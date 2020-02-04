#include "ShaderManager.h"
#include <vector>
#include "MyLog.h"
#include "glUtil.h"
#include <cassert>

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

	uniform_[s][SU_VIEWPROJ] = glGetUniformLocation(program_[s], "mViewProj");
	GLog.LogInfo("uniform_[SU_VIEWPROJ] %d", uniform_[s][SU_VIEWPROJ]);

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

void ShaderManager::SetUnifrom(ShaderUniform u, const Matrix4f& matrix)
{
	if (uniform_[currentBind_][u] != -1) {
		glUniformMatrix4fv(uniform_[currentBind_][u], 1, GL_FALSE, matrix.Ptr());
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



void CST_CreateShader();
void CST_CreateBuffer();
void CST_Run(float time);
void CST_TestValue();

bool CST_CreateParticleShader();

void ShaderManager::TestComputeShader()
{
	CST_CreateShader();
	CST_CreateBuffer();
		
	char* fileName = "../../assets/Particle.glsl";
	LoadFromFile(ShaderParticle, fileName);

	//CST_CreateParticleShader();
}





GLuint GComputeShader = 0;

GLuint position_buffer;
GLuint velocity_buffer;
GLuint position_tbo;
GLuint velocity_tbo;
GLuint uni_buffer_tractor;

GLuint uniform_index_dt;


GLuint particle_shader;

GLuint vertex_array_particle;


const int NUM_COMPUTE_GROUP = 1000;
int PARTICLE_COUNT = 128 * 1000;
const static int TRACTOR_COUTNT = 32;



void ShaderManager::RunComputerShader(float time, const Matrix4f& matViewProj)
{
	CST_Run(time);
	//CST_TestValue();

	Bind(ShaderParticle);
	SetUnifrom(SU_VIEWPROJ, matViewProj);

	glBindVertexArray(vertex_array_particle);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);

}




float tractor_mass[TRACTOR_COUTNT];
float TotalTime = 0.0f;
float particle_range = 20.f;


void CST_CreateShader()
{
	GLuint shader, program;
	GLint state = 0;

	char* fileName = "../../assets/ComputerShader.glsl";

	vector<char> code;
	bool bRes = LoadFile(code, fileName);
	if (!bRes) {

		assert(0);
		return;
	}

	const char* src[] = { &code[0] };

	shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, src, NULL);

	bRes = CompileShader(shader, "", &code[0]);
	if (!bRes) {

		assert(0);
		return;
	}

	program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &state);
	if (state != GL_TRUE) {
		GLog.LogError("link shader failed!");

		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 0) {
			vector<char> err(infoLen + 1);
			glGetProgramInfoLog(program, infoLen + 1, NULL, &err[0]);
			GLog.LogError(&err[0]);
		}

		assert(0);
		return;
	}
	
	GComputeShader = program;
}




void CST_CreateBuffer()
{
	glGenVertexArrays(1, &vertex_array_particle);
	glBindVertexArray(vertex_array_particle);


	// Generate two buffers, bind them and initialize their data stores
	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(Vector4f), NULL, GL_DYNAMIC_COPY);
	// Map the position buffer and fill it with random vectors
	Vector4f* positions = (Vector4f*)glMapBufferRange(GL_ARRAY_BUFFER,
			0, PARTICLE_COUNT * sizeof(Vector4f),
			GL_MAP_WRITE_BIT |
			GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		positions[i] = Vector4f(
			Mathf::Random(-particle_range, particle_range),
			Mathf::Random(-particle_range, particle_range),
			Mathf::Random(-particle_range, particle_range),
			Mathf::Random01());
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4f), BUFFER_OFFSET(0));

	glBindVertexArray(0);



	glGenTextures(1, &position_tbo);
	glBindTexture(GL_TEXTURE_BUFFER, position_tbo);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, position_buffer);


	glGenBuffers(1, &velocity_buffer);
	// Initialization of the velocity buffer - also filled with random vectors
	glBindBuffer(GL_ARRAY_BUFFER, velocity_buffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(Vector4f), NULL, GL_DYNAMIC_COPY);
	Vector4f* velocities = (Vector4f*)glMapBufferRange(GL_ARRAY_BUFFER,
		0, PARTICLE_COUNT * sizeof(Vector4f),
		GL_MAP_WRITE_BIT |
		GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		velocities[i] = Vector4f(
			Mathf::Random(-1.f, 1.f),
			Mathf::Random(-1.f, 1.f),
			Mathf::Random(-1.f, 1.f),
			0.f
		);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	glGenTextures(1, &velocity_tbo);
	glBindTexture(GL_TEXTURE_BUFFER, velocity_tbo);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, velocity_buffer);


	glGenBuffers(1, &uni_buffer_tractor);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uni_buffer_tractor);
	//glBindBuffer(GL_UNIFORM_BUFFER, uni_buffer_tractor);

	glBufferData(GL_UNIFORM_BUFFER, TRACTOR_COUTNT * sizeof(Vector4f), NULL, GL_DYNAMIC_COPY);

	for (int i = 0; i < TRACTOR_COUTNT; i++) {
		tractor_mass[i] = Mathf::Random(999.f, 9999.f);
	}

	GLog.LogInfo("tractor: %f", tractor_mass[0]);

	GLuint blockIndex = glGetUniformBlockIndex(GComputeShader, "attractor_block");
	GLog.LogInfo("block index: %d", blockIndex);

	uniform_index_dt = glGetUniformLocation(GComputeShader, "dt");
	//GLog.LogInfo("uniform index dt: %d", uniform_index_dt);
}




bool CST_CreateParticleShader()
{
	char* fileName = "../../assets/Particle.glsl";

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

	particle_shader = glCreateProgram();
	glAttachShader(particle_shader, vs);
	glAttachShader(particle_shader, ps);
	glLinkProgram(particle_shader);

	glGetProgramiv(particle_shader, GL_LINK_STATUS, &state);
	if (state != GL_TRUE) {
		GLog.LogError("link shader failed!");

		GLint infoLen = 0;
		glGetProgramiv(particle_shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 0) {
			vector<char> err(infoLen + 1);
			glGetProgramInfoLog(particle_shader, infoLen + 1, NULL, &err[0]);
			GLog.LogError(&err[0]);
		}

		assert(0);
		return false;
	}

}





void CST_Run(float time)
{
	TotalTime += time * 0.001f;
	// Update the buffer containing the attractor positions and masses

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uni_buffer_tractor);

	Vector4f* attractors =
		(Vector4f*)glMapBufferRange(GL_UNIFORM_BUFFER,
			0, TRACTOR_COUTNT * sizeof(Vector4f),
			GL_MAP_WRITE_BIT |
			GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < TRACTOR_COUTNT; i++)
	{
		attractors[i] =
			Vector4f(sinf(TotalTime * (float)(i + 4) * 7.5f) * 50.0f,
				cosf(TotalTime * (float)(i + 7) * 3.9f) * 50.0f,
				sinf(TotalTime * (float)(i + 3) * 5.3f) *
				cosf(TotalTime * (float)(i + 5) * 9.1f) * 100.0f,
				tractor_mass[i]);
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);


	glUseProgram(GComputeShader);
	glBindImageTexture(0, position_tbo, 0,
		GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, velocity_tbo, 0,
		GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glUniform1f(uniform_index_dt, time);

	glDispatchCompute(NUM_COMPUTE_GROUP, 1, 1);
	// Ensure that writes by the compute shader have completed
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}




void CST_TestValue()
{
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	Vector4f* positions = (Vector4f*)glMapBufferRange(GL_ARRAY_BUFFER,
		0, PARTICLE_COUNT * sizeof(Vector4f),
		GL_MAP_READ_BIT);

	for (int i = 0; i < 1; i++) {
		GLog.LogInfo("%d %.3f %.3f %.3f %.3f", i,
			positions[i].x, positions[i].y, positions[i].z, positions[i].w);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

}