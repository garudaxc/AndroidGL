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
#include "Texture.h"
#include "Timer.h"
#include "GlobalVar.h"
#include "BitmapFont.h"
#include "SpriteBatch.h"
#include "Calibration.h"
#include "FileSystem.h"

using namespace std;
using namespace FancyTech;

struct glState_t glState;

#define TEST_MODEL 1


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
BitmapFont bitmapFont;
SpriteBatch spriteBatch;


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

	GFileSys->SetRootPath("/sdcard/MyTest/", true);

	glState.width = w;
	glState.height = h;

	LogGLInfo();
	GShaderManager.LoadFromFile(ShaderDiffuse, "/sdcard/MyTest/shader.glsl");
	GShaderManager.LoadFromFile(ShaderUI, "/sdcard/MyTest/ShaderUI.glsl");

	bitmapFont.LoadFromFile("consolas.bitmapfont");
	spriteBatch.Init(256);

	ModelInstance* model = NULL;
	//model = CreateModel("build_tower003.mesh", "1.png");
	//model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(-1.0f, 1.0f, 0.0f));
	//Models.push_back(model);

	//model = CreateModel("build_house008.mesh", "1.png");
	//model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(1.5f, 2.0f, 0.0f));
	//Models.push_back(model);

#if TEST_MODEL
	model = CreateModel("Box01.mesh", "2.png");
	model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(0.0f, 0.0f, 0.0f));
	Models.push_back(model);
#else
	model = CreateModel("Box001.mesh", "2.png");
	model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(0.0f, 0.0f, 0.0f));
	Models.push_back(model);
#endif

	checkGlError("CreateModel");
	GLog.LogInfo("create model finished");
	return 1;
}


Matrix4f _GetDeviceRotationMatrix();

TrackerSample drawSample;
Vector3f smoothAccel_;

void DrawTrackSample(const TrackerSample& sample)
{
	drawSample = sample;
	float alpha = 0.01f;
	smoothAccel_ = sample.accelerate * alpha + smoothAccel_ * (1.0f - alpha);
}


extern int readlen;

void DrawView(int x, int y, int w, int h, float eyeOffset)
{
	glViewport(x, y, w, h);

	Matrix4f mView, mEyeOffset;

#if TEST_MODEL
	mView = Matrix4f::LookAtRH(Vector3f(0.f, -3.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f::UNIT_Z);
#else
	mView = _GetDeviceRotationMatrix();
#endif
	
	Vector3f eyePos(0.f, 0.f, 0.f);
	mView = Matrix4f::Translate(-eyePos) * mView;
	mView *= Matrix4f::Translate(Vector3f(eyeOffset, 0.f, 0.f));
	
	Matrix4f mProj = Matrix4f::PerspectiveFovRH(Mathf::PI / 3.0f, w / (float)h, 0.1f);
	mProj._33 = mProj._33 * 2.f + mProj._34 * -1.f;
	mProj._43 = mProj._43 * 2.f;

	for (auto it = Models.begin(); it != Models.end(); ++it) {		

#if TEST_MODEL
		Matrix4f mWorld = _GetDeviceRotationMatrix();
#else
		Matrix4f mWorld = Matrix4f::IDENTITY;
#endif
		//mWorld = (*it)->transform_;

		GShaderManager.Bind(ShaderDiffuse);
		GShaderManager.SetUnifrom(SU_WORLD, mWorld);
		GShaderManager.SetUnifrom(SU_VIEW, mView);
		GShaderManager.SetUnifrom(SU_PROJECTION, mProj);
		GShaderManager.SetUnifrom(SU_TEX_DIFFUSE, 0);

		glActiveTexture(GL_TEXTURE0);
		(*it)->texture_.Bind();
				
		Model& mesh = (*it)->mesh_;
		//mesh.Bind();
		//for (int i = 0; i < mesh.GetElementCount(); i++) {
		//	const ModelElement* e = mesh.GetElement(i);

		//	void* index = (void*)(e->indexOffset * sizeof(unsigned short));
		//	glDrawElements(GL_TRIANGLES, e->indexCount, GL_UNSIGNED_SHORT, index);
		//}
	}
}



void DrawCalibriation(int w, int h)
{
	float lineHeight = -30.0f;
	Vector3f pos(10.f, h - 10.f, 0.f);

	char buff[64];
	sprintf(buff, "fps %.2f", Time.GetFPS());
	bitmapFont.DrawString(&spriteBatch, buff, pos);

	pos.y += lineHeight;
	sprintf(buff, "samples %d", GCalibration.GetNumSamples());
	bitmapFont.DrawString(&spriteBatch, buff, pos);
	spriteBatch.Commit(w, h);

	pos.y += lineHeight;
	sprintf(buff, "accel    %+.4f %+.4f %+.4f", smoothAccel_.x, smoothAccel_.y, smoothAccel_.z);
	bitmapFont.DrawString(&spriteBatch, buff, pos);

	pos.y += lineHeight;
	sprintf(buff, "gyro(c)  %+.4f %+.4f %+.4f", drawSample.gyro.x, drawSample.gyro.y, drawSample.gyro.z);
	bitmapFont.DrawString(&spriteBatch, buff, pos);
	spriteBatch.Commit(w, h);

	pos.y += lineHeight;
	if (GCalibration.IsCalibrated()){
		Vector3f offset = GCalibration.GetOffest();
		sprintf(buff, "Temperature %.2f offset (%.3f %.3f %.3f)", GCalibration.GetTemperature(), offset.x, offset.y, offset.z);
		bitmapFont.DrawString(&spriteBatch, buff, pos);
	}

	spriteBatch.Commit(w, h);
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

	//DrawView(0, 0, glState.width / 2, glState.height, eyeDistance / 2.f);
	//DrawView(glState.width / 2, 0, glState.width / 2, glState.height, -eyeDistance / 2.f);

	DrawCalibriation(glState.width, glState.height);

	if (Time.GetTime() - lastTime > 1.f) {
		GLog.LogInfo("fps %f time %f", Time.GetFPS(), Time.GetTime());
		lastTime = Time.GetTime();
	}
	
}
