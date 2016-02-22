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
#include "Input.h"
#include "audio/audio.h"
#include "jni.h"

using namespace std;
using namespace FancyTech;

struct glState_t glState;

#define TEST_MODEL 1



GlobalVar EyeDistance("EyeDistance", "0.4f", GVFLAG_FLOAT, "");

//extern "C" JNIEXPORT void JNICALL LibTest();
extern "C" JNIEXPORT void JNICALL init(JNIEnv *env, jobject thiz, jint width, jint height, jobject surface);


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

	GLog.LogInfo("begin setupGraphics");	
	//init(NULL, NULL, w, h, NULL);
	GLog.LogInfo("after init");

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
	model = CreateModel("build_tower003.mesh", "1.png");
	model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(-1.0f, 1.0f, 0.0f));
	Models.push_back(model);

	model = CreateModel("build_house008.mesh", "1.png");
	model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(1.5f, 2.0f, 0.0f));
	Models.push_back(model);

//#if TEST_MODEL
//	model = CreateModel("Box01.mesh", "2.png");
//	model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(0.0f, 0.0f, 0.0f));
//	Models.push_back(model);
//#else
//	model = CreateModel("Box001.mesh", "2.png");
//	model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(0.0f, 0.0f, 0.0f));
//	Models.push_back(model);
//#endif

	checkGlError("CreateModel");
	GLog.LogInfo("create model finished");

	return 1;
}


Matrix4f _GetDeviceRotationMatrix();


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
		mesh.Bind();
		for (int i = 0; i < mesh.GetElementCount(); i++) {
			const ModelElement* e = mesh.GetElement(i);

			void* index = (void*)(e->indexOffset * sizeof(unsigned short));
			glDrawElements(GL_TRIANGLES, e->indexCount, GL_UNSIGNED_SHORT, index);
		}
	}
}

namespace FancyTech
{
	void DrawCalibration(int w, int h, BitmapFont& bitmapFont, SpriteBatch& spriteBatch);
}


float lastTime = 0.f;
//
bool audioInited = false;
int	bpm_ = 0;
bool toutched_ = false;
//
//class LogGyroTransform : public EventReceiver
//{
//public:
//
//	bool	OnEvent(const Event& event)	{
//		if (event.Type == Event::LButtonUp ||
//			event.Type == Event::TouchUp)
//		{
//			toutched_ = true;
//
//			if (!audioInited) {
//				bool succeeded = false;
//				if (event.fxPos < glState.width / 2) {
//					succeeded = GAudioSystem.Init("100bpm.wav");
//					bpm_ = 100;
//				} else {
//					succeeded = GAudioSystem.Init("140bpm.wav");
//					bpm_ = 140;
//				}
//
//				if (!succeeded)	{
//					GLog.LogInfo("Audio init failed");
//				} else {
//					audioInited = true;
//				}
//				
//				if (audioInited) {
//					GAudioSystem.Play();
//				}
//			}
//		}
//
//		return true;
//	}
//
//};

//static LogGyroTransform logger;

extern Vector3f gyro_;

void renderFrame() {

	Vector3f color = Vector3f::ZERO;
	if (toutched_)	{
		color = Vector3f::UNIT_X;
		toutched_ = false;
	}

	glClearColor(color.x, color.y, color.z, 1.f);
	glClearDepthf(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	float lineHeight = -30.0f;
	Vector3f pos(10.f, glState.height - 10.f, 0.f);

	char buff[64];
	//sprintf(buff, "fps %.2f   %d bpm", Time.GetFPS(), bpm_);
	//bitmapFont.DrawString(&spriteBatch, buff, pos);
	//spriteBatch.Commit(glState.width, glState.height);


	pos.Set(10.f, glState.height / 2, 0.f);
	sprintf(buff, "%+.4f %+.4f %+.4f", gyro_.x, gyro_.y, gyro_.z);
	bitmapFont.DrawString(&spriteBatch, buff, pos, 1.f);
	spriteBatch.Commit(glState.width, glState.height);

	if (!audioInited) {
		return;
	}
	float eyeDistance = EyeDistance.GetFloat();

	DrawView(0, 0, glState.width / 2, glState.height, eyeDistance / 2.f);
	DrawView(glState.width / 2, 0, glState.width / 2, glState.height, -eyeDistance / 2.f);

	//if (!audioInited) {
	//	return;
	//}

	//pos.Set(glState.width / 2, glState.height / 2, 0.f);
	//sprintf(buff, "%u", GAudioSystem.GetPosition());
	//bitmapFont.DrawString(&spriteBatch, buff, pos, 5);


	//DrawCalibration(glState.width, glState.height, bitmapFont, spriteBatch);

	//if (Time.GetTime() - lastTime > 1.f) {
	//	GLog.LogInfo("fps %f time %f", Time.GetFPS(), Time.GetTime());
	//	lastTime = Time.GetTime();
	//}
	
}


