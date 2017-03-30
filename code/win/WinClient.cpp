#include "Client.h"
#include "Rendering.h"
#include "ShaderManager.h"
#include "Timer.h"
#include "AuroraGL.h"
#include "FileSystem.h"
#include "glUtil.h"
#include "input.h"

using namespace FancyTech;


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
//			/*if (!audioInited) {
//			bool succeeded = false;
//			if (event.fxPos < glState.width / 2) {
//			succeeded = GAudioSystem.Init("100bpm.wav");
//			bpm_ = 100;
//			} else {
//			succeeded = GAudioSystem.Init("140bpm.wav");
//			bpm_ = 140;
//			}
//
//			if (!succeeded)	{
//			GLog.LogInfo("Audio init failed");
//			} else {
//			audioInited = true;
//			}
//
//			if (audioInited) {
//			GAudioSystem.Play();
//			}
//			}*/
//		}
//
//		return true;
//	}
//
//};


class MyClient : public IClient, public EventReceiver
{

private:
	vector<ModelInstance*>		Models;
	ViewParam		viewParam_;
	Texture		renderTexture_;
	FrameBuffer		frameBuffer_;

public:
	MyClient();
	~MyClient();

	virtual void OnCreate() override
	{
		GShaderManager.LoadFromFile(ShaderDiffuse, "../../assets/shader330.glsl");
		GShaderManager.LoadFromFile(ShaderUI, "../../assets/ShaderUI.glsl");
		checkGlError("GShaderManager.LoadFromFile");

		ModelInstance* model = NULL;
		model = CreateModel("Plane01.mesh", "1.png");
		model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(-1.0f, 1.0f, 0.0f));
		Models.push_back(model);

		model = CreateModel("build_house008.mesh", "1.png");
		model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(1.5f, 2.0f, 0.0f));
		Models.push_back(model);

		viewParam_.clearColor.Set(0.3f, 0.3f, 0.3f, 1.0f);
		viewParam_.clearDepth = 1.0;

		//renderTexture_.Create(RenderSystem->GetCurrentWindow().width,
		//	RenderSystem->GetCurrentWindow().height, RGBA8);

		//Models[0]->texture_ = renderTexture_;


		frameBuffer_.Create(1024, 1024, RGB8, 0);
	}

	virtual void OnUpdate() override
	{
	}

	virtual void OnRender() override
	{
		//glActiveTexture(GL_TEXTURE0);
		//renderTexture_.Bind();
		//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0,
		//	RenderSystem->GetCurrentWindow().width,
		//	RenderSystem->GetCurrentWindow().height, 0);
		//checkGlError("glCopyTexImage2D");

		Matrix4f mView = Matrix4f::LookAtRH(Vector3f(0.f, -1.5f, 0.3f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f::UNIT_Z);

		Matrix4f mProj = Matrix4f::PerspectiveFovRH(Mathf::PI / 3.0f, glState.width / (float)glState.height, 0.1f);
		mProj._33 = mProj._33 * 2.f + mProj._34 * -1.f;
		mProj._43 = mProj._43 * 2.f;

		viewParam_.ViewMatrix = mView;
		viewParam_.ProjMatrix = mProj;
		RenderSystem->BeginView(viewParam_);

		for (auto it = Models.begin(); it != Models.end(); ++it) {

			Matrix4f mWorld = Matrix4f::RotationAxis(Vector3f::UNIT_Z, Time.GetTime() * 0.2f);
			mWorld *= (*it)->transform_;

			GShaderManager.Bind(ShaderDiffuse);
			GShaderManager.SetUnifrom(SU_WORLD, mWorld);
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

	virtual void OnDestroy() override
	{
	}

	virtual bool OnEvent(const Event& event) override
	{
		if (event.Type == Event::LButtonUp ||
			event.Type == Event::TouchUp)
		{
			frameBuffer_.Bind();
			RenderSystem->BeginFrame();
			OnRender();
			RenderSystem->EndFrame();
			frameBuffer_.Unbind();
			checkGlError("render frame buffer");
			
			Models[0]->texture_ = frameBuffer_.GetTexture();
		}

		return true;
	}

};

MyClient::MyClient()
{
}

MyClient::~MyClient()
{
}

static MyClient client;
IClient* MainClient = &client;