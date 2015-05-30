#include <windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include "glew.h"
#include "AuroraGL.h"
#include "MyLog.h"

#include "MathFunction.h"
#include "Matrix.h"
#include "Model.h"
#include "ShaderManager.h"

using namespace std;
using namespace Aurora;


GLuint texture;
GLuint texture2;

Model model;


#define COLOR(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)


void InitTexture() {

	// The following is an 8x8 checkerboard pattern using
	// GL_RED, GL_UNSIGNED_BYTE data.
	static const GLubyte tex_checkerboard_data[] =
	{
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
	};
	// The following data represents a 2x2 texture with red,
	// green, blue, and yellow texels represented as GL_RGBA,
	// GL_FLOAT data.
	static const GLfloat tex_color_data[] =
	{
		// Red texel Green texel
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		// Blue texel Yellow texel
		0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	const GLuint texcolor[] = {
		COLOR(0, 0, 0, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(255, 0, 0, 255),
		COLOR(255, 0, 0, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(128, 128, 128, 255),
		COLOR(255, 0, 0, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(255, 0, 0, 255),
		COLOR(255, 255, 255, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(255, 0, 0, 255),
	};


	glGenTextures(1, &texture);

	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, texcolor);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, 8, 8);

	glTexSubImage2D(GL_TEXTURE_2D,
		0,
		0, 0,
		8, 8,
		GL_RED, GL_UNSIGNED_BYTE,
		tex_checkerboard_data);

	static const GLint swizzles[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void LoadResource() {

	GShaderManager.LoadFromFile("../../assets/shader330.glsl");

	model.Load(NULL);


	InitTexture();
}

void DrawFrame() {

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);


	////float scale = (sinf(timer.GetTime()) + 1.f) * 0.5f;
	//float scale = sinf(timer.GetTime() * 0.5f);

	//Matrix4f matRot = Matrix4f::IDENTITY;
	////matRot._41 = scale;

	//MatrixRotationAxis(matRot, Vector3f::UNIT_Y, scale);

	//static unsigned int oldTime = GetTickCount();
	unsigned int time = GetTickCount();
	static float angle = 0.f;

	Matrix4f mWorld;
	MatrixRotationAxis(mWorld, Vector3f::UNIT_Z, angle);

	angle += 0.01f;	
	
	Matrix4f mView, mProj;
	MatrixLookAtRH(mView, Vector3f(0.f, -3.0f, 0.0f), Vector3f::ZERO, Vector3f::UNIT_Z);
	MatrixPerspectiveFovRH(mProj, Mathf::PI / 4.0f, glState.width / (float)glState.height, 0.1f);
	mProj._33 = mProj._33 * 2.f + mProj._34 * -1.f;
	mProj._43 = mProj._43 * 2.f;

	GShaderManager.Bind();
	GShaderManager.SetUnifrom(SU_WORLD, mWorld.Ptr());
	GShaderManager.SetUnifrom(SU_VIEW, mView.Ptr());
	GShaderManager.SetUnifrom(SU_PROJECTION, mProj.Ptr());
	GShaderManager.SetUnifrom(SU_TEX_DIFFUSE, 0);
	GShaderManager.SetUnifrom(SU_TEX_NORMAL, 1);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//if (keyDown % 2)
	//{
	//}
	//else
	//{
	//	glBindTexture(GL_TEXTURE_2D, texture2);
	//}


	model.Bind();
	for (int i = 0; i < model.GetElementCount(); i++) {
		const ModelElement* e = model.GetElement(i);

		void* index = (void*)(e->faceOffest * 3 * sizeof(unsigned short));
		//glDrawElements(GL_TRIANGLES, e->faceCount * 3, GL_UNSIGNED_INT, index);
		glDrawElementsBaseVertex(GL_TRIANGLES, e->faceCount * 3, GL_UNSIGNED_SHORT, index, e->vertexOffset);
	}
	
	glBindVertexArray(0);

	glFlush();
	glFinish();

	SwapBuffers(glState.hdc);
}