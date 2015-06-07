#include <string>
#include <vector>
#include <stdio.h>
#include "Platfrom.h"
#include <stdio.h>
#include <stdlib.h>
#include "AuroraGL.h"
#include "MyLog.h"
#include "glUtil.h"
#include "ShaderManager.h"
#include "Model.h"
#include "AurMath.h"
#include "Matrix.h"
#include "Vector.h"
#include "MathFunction.h"
#include "Texture.h"
#include "Timer.h"
#include "GlobalVar.h"

using namespace std;
using namespace Aurora;

struct glState_t glState;


GlobalVar EyeDistance("EyeDistance", "0.4f", GVFLAG_FLOAT, "");

class ModelInstance
{
public:
	Model		mesh_;
	Texture		texture_;
	Matrix4f	transform_;

private:

};

vector<ModelInstance*>		Models;

ModelInstance*	CreateModel(const char* mesh, const char* texture)
{
	ModelInstance* model = new ModelInstance;
	model->mesh_.Load(mesh);
	model->texture_.Load(texture);

	model->transform_ = Matrix4f::IDENTITY;

	return model;
}

int setupGraphics(int w, int h) {
	GGlobalVarManager->Init();

	glState.width = w;
	glState.height = h;

	LogGLInfo();
	GShaderManager.LoadFromFile("/sdcard/MyTest/shader.glsl");

	ModelInstance* model = NULL;
	//model = CreateModel("/sdcard/MyTest/build_tower003.mesh", "/sdcard/MyTest/other_stone001_d.TGA");
	//MatrixTransform(model->transform_, Quaternionf::IDENTITY, Vector3f(-1.0f, 0.0f, 0.0f));
	//Models.push_back(model);

	//model = CreateModel("/sdcard/MyTest/build_house008.mesh", "/sdcard/MyTest/other_stone001_d.TGA");
	//MatrixTransform(model->transform_, Quaternionf::IDENTITY, Vector3f(1.5f, 0.0f, 0.0f));
	//Models.push_back(model);


	model = CreateModel("/sdcard/MyTest/Box001.mesh", "/sdcard/MyTest/2.png");
	MatrixTransform(model->transform_, Quaternionf::IDENTITY, Vector3f(0.0f, 0.0f, 0.0f));
	Models.push_back(model);
	
	checkGlError("CreateModel");
	return 1;
}


Matrix4f _GetDeviceRotationMatrix();

void DrawView(int x, int y, int w, int h, float eyeOffset)
{
	glViewport(x, y, w, h);

	Matrix4f mView, mEyeOffset, mProj;

	//MatrixLookAtRH(mView, Vector3f(0.f, -6.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f::UNIT_Z);

	mView = _GetDeviceRotationMatrix();
	Vector3f eyePos(0.f, 0.f, 0.f);
	Matrix4f matEyePos;
	MatrixTranslation(matEyePos, -eyePos);
	MatrixMultiply(mView, matEyePos, mView);
	
	MatrixTranslation(mEyeOffset, Vector3f(eyeOffset, 0.f, 0.f));
	MatrixMultiply(mView, mView, mEyeOffset);

	MatrixPerspectiveFovRH(mProj, Mathf::PI / 3.0f, w / (float)h, 0.1f);
	mProj._33 = mProj._33 * 2.f + mProj._34 * -1.f;
	mProj._43 = mProj._43 * 2.f;

	for (vector<ModelInstance*>::iterator it = Models.begin(); it != Models.end(); ++it) {
		
		Matrix4f mWorld = Matrix4f::IDENTITY;
		//MatrixRotationAxis(mWorld, Vector3f::UNIT_Z, Time.GetTime() * 0.2f);
		MatrixMultiply(mWorld, mWorld, (*it)->transform_);

		GShaderManager.Bind();
		GShaderManager.SetUnifrom(SU_WORLD, mWorld.Ptr());
		GShaderManager.SetUnifrom(SU_VIEW, mView.Ptr());
		GShaderManager.SetUnifrom(SU_PROJECTION, mProj.Ptr());
		GShaderManager.SetUnifrom(SU_TEX_DIFFUSE, 0);

		glActiveTexture(GL_TEXTURE0);
		(*it)->texture_.Bind();
		
		Model& mesh = (*it)->mesh_;
		mesh.Bind();
		for (int i = 0; i < mesh.GetElementCount(); i++) {
			const ModelElement* e = mesh.GetElement(i);

			void* index = (void*)(e->indexOffset * sizeof(unsigned short));
			glDrawElements(GL_TRIANGLES, e->indexCount, GL_UNSIGNED_SHORT, index);
		}
	}
}


float lastTime = 0.f;

void renderFrame() {

	glClearColor(0.3f, 0.3f, 0.3f, 0);
	glClearDepthf(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	float eyeDistance = EyeDistance.GetFloat();

	DrawView(0, 0, glState.width / 2, glState.height, eyeDistance / 2.f);
	DrawView(glState.width / 2, 0, glState.width / 2, glState.height, -eyeDistance / 2.f);

	if (Time.GetTime() - lastTime > 1.f) {
		GLog.LogInfo("%f %f", Time.GetFPS(), Time.GetTime());
		lastTime = Time.GetTime();
	}
	
}
