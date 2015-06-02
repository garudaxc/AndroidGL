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
#include "Texture.h"
#include "glUtil.h"

using namespace std;
using namespace Aurora;


Model model;

Texture tex;
Texture tex2;


#define COLOR(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)


void LoadResource() {

	GShaderManager.LoadFromFile("../../assets/shader330.glsl");
	checkGlError("GShaderManager.LoadFromFile");

	tex.Load("2.png");
	tex2.Load("1.png");

	model.Load("OilTank001.mesh");

	//InitTexture();
}

void DrawFrame() {

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


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
	tex.Bind();

	glActiveTexture(GL_TEXTURE1);
	tex2.Bind();

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

		void* index = (void*)(e->indexOffset * sizeof(unsigned short));
		//glDrawElements(GL_TRIANGLES, e->faceCount * 3, GL_UNSIGNED_INT, index);
		glDrawElementsBaseVertex(GL_TRIANGLES, e->indexCount, GL_UNSIGNED_SHORT, index, e->vertexOffset);
	}
	
	glBindVertexArray(0);

	glFlush();
	glFinish();

	SwapBuffers(glState.hdc);
}