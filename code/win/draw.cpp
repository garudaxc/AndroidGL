#include <windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include "glew.h"
#include "AuroraGL.h"
#include "MyLog.h"
#include "AurMath.h"
#include "Model.h"
#include "ShaderManager.h"
#include "Texture.h"
#include "glUtil.h"
#include "Timer.h"
#include "GlobalVar.h"
#include "BitmapFont.h"
#include "SpriteBatch.h"
#include "SensorDevice.h"
#include "Calibration.h"
#include "FileSystem.h"
#include "Input.h"

using namespace std;
using namespace FancyTech;


//class MyReceiver : public EventReceiver
//{
//public:
//
//	bool	OnEvent(const Event& event)	{
//		if (event.Type != Event::MouseMove)
//		{
//			GLog.LogInfo("%hd, %d %d", event.Type, event.xPos, event.yPos);
//		}
//		return true;
//	}
//
//};
//
//MyReceiver a;



GlobalVar EyeDistance("EyeDistance", "0.065f", GVFLAG_FLOAT, "");

#define COLOR(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)

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



void LoadResource() {
	GGlobalVarManager->Init();

	GFileSys->SetRootPath("../../resource/", true);
	
	GShaderManager.LoadFromFile(ShaderDiffuse, "../../assets/shader330.glsl");
	GShaderManager.LoadFromFile(ShaderUI, "../../assets/ShaderUI.glsl");
	checkGlError("GShaderManager.LoadFromFile");

	bitmapFont.LoadFromFile("consolas.bitmapfont");
	spriteBatch.Init(128);

	ModelInstance* model = NULL;
	model = CreateModel("build_tower003.mesh", "2.png");
	model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(-1.0f, 10.0f, 0.0f));
	Models.push_back(model);

	//model = CreateModel("build_house008.mesh", "1.png");
	//model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(1.5f, 10.0f, 0.0f));
	//Models.push_back(model);


	//model = CreateModel("Box001.mesh", "2.png");
	//MatrixTransform(model->transform_, Quaternionf::IDENTITY, Vector3f(0.0f, 0.0f, 0.0f));
	//Models.push_back(model);

	GGyroCalibration.Init("8D8651A65153");
	GGyroCalibration.Load();
	GGyroCalibration.Save();

	libUsbTest();
}

void UnloadResource()
{
	StopTrackerThread();
}

extern int sampleCount;


void DrawOneEye(int x, int y, int w, int h, float eyeOffset)
{
	glViewport(x, y, w, h);

	Matrix4f mView = Matrix4f::LookAtRH(Vector3f(0.f, -3.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f::UNIT_Z);
	mView *= Matrix4f::Translate(Vector3f(eyeOffset, 0.f, 0.f));


	Matrix4f mProj = Matrix4f::PerspectiveFovRH(Mathf::PI / 3.0f, w / (float)h, 0.1f);
	mProj._33 = mProj._33 * 2.f + mProj._34 * -1.f;
	mProj._43 = mProj._43 * 2.f;
	
	for (auto it = Models.begin(); it != Models.end(); ++it) {

		Matrix4f mWorld = Matrix4f::RotationAxis(Vector3f::UNIT_Z, Time.GetTime() * 0.2f);
		mWorld *= (*it)->transform_;

		GShaderManager.Bind(ShaderDiffuse);
		GShaderManager.SetUnifrom(SU_WORLD, mWorld);
		GShaderManager.SetUnifrom(SU_VIEW, mView);
		GShaderManager.SetUnifrom(SU_PROJECTION, mProj);
		GShaderManager.SetUnifrom(SU_TEX_DIFFUSE, 0);
		GShaderManager.SetUnifrom(SU_TEX_NORMAL, 1);

		glActiveTexture(GL_TEXTURE0);
		(*it)->texture_.Bind();

		glActiveTexture(GL_TEXTURE1);
		(*it)->texture_.Bind();
				
		Model& mesh = (*it)->mesh_;
		mesh.Bind();
		for (int i = 0; i < mesh.GetElementCount(); i++) {
			const ModelElement* e = mesh.GetElement(i);

			void* index = (void*)(e->indexOffset * sizeof(unsigned short));
			//glDrawElements(GL_TRIANGLES, e->faceCount * 3, GL_UNSIGNED_INT, index);
			glDrawElementsBaseVertex(GL_TRIANGLES, e->indexCount, GL_UNSIGNED_SHORT, index, e->vertexOffset);
		}
	}

	glBindVertexArray(0);

}

namespace FancyTech{

	void DrawCalibration(int w, int h, BitmapFont& bitmapFont, SpriteBatch& spriteBatch);
}


void DrawFrame() {
	
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	float eyeDistance = EyeDistance.GetFloat();

	//DrawView(0, 0, glState.width / 2, glState.height, eyeDistance / 2.f);
	//DrawView(glState.width / 2, 0, glState.width / 2, glState.height, -eyeDistance / 2.f);
	DrawCalibration(glState.width, glState.height, bitmapFont, spriteBatch);

	glFlush();
	glFinish();

	SwapBuffers(glState.hdc);
}