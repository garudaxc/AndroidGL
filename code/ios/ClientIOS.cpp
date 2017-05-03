#include "Client.h"
#include "Rendering.h"
#include "ShaderManager.h"
#include "Timer.h"
//#include "AuroraGL.h"
#include "MyLog.h"
#include "glUtil.h"
#include "FileSystem.h"
#include "Input.h"
#include "Rendering.h"

using namespace FancyTech;


uint32_t dua = 0;




class MyClient : public IClient, public EventReceiver
{

private:
	vector<ModelInstance*>		Models;
	ViewParam		viewParam_;
	Texture		renderTexture_;
	int state_;

	Texture texTest;

	FrameBuffer		frameBuffer_;

public:
	MyClient();
	~MyClient();

	virtual void OnCreate() override
	{
		state_ = 0;

		LogGLInfo();
		GShaderManager.LoadFromFile(ShaderDiffuse, "/sdcard/MyTest/shader.glsl");
		GShaderManager.LoadFromFile(ShaderUI, "/sdcard/MyTest/ShaderUI.glsl");

		ModelInstance* model = NULL;
		model = CreateModel("Plane01.mesh", "1.png");
		model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(-1.0f, 1.0f, 0.0f));
		Models.push_back(model);

		model = CreateModel("build_house008.mesh", "1.png");
		model->transform_ = Matrix4f::Transform(Quaternionf::IDENTITY, Vector3f(1.5f, 2.0f, 0.0f));
		Models.push_back(model);

		viewParam_.clearColor.Set(0.3f, 0.3f, 0.3f, 1.0f);
		viewParam_.clearDepth = 1.0;

		checkGlError("CreateModel");
		GLog.LogInfo("create model finished");

	}

	virtual void OnUpdate() override
	{
	}

	virtual void OnRender() override
	{
		if (state_ == 2)
		{
			uint32_t t = Time.GetTimeMS();
			glActiveTexture(GL_TEXTURE0);
			renderTexture_.Bind();
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0,
				RenderSystem->GetCurrentWindow().width,
				RenderSystem->GetCurrentWindow().height, 0);

			dua = Time.GetTimeMS() - t;
			checkGlError("glCopyTexImage2D");
        }
        
        const RenderWindow& w = RenderSystem->GetCurrentWindow();


		Matrix4f mView = Matrix4f::LookAtRH(Vector3f(0.f, -1.5f, 0.3f),
                                            Vector3f(0.0f, 0.0f, 0.0f),
                                            Vector3f::UNIT_Z);

		Matrix4f mProj = Matrix4f::PerspectiveFovRH(Mathf::PI / 3.0f, w.width / (float)w.height, 0.1f);
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
				
		Vector3f pos(10.f, RenderSystem->GetCurrentWindow().height - 50.f, 0.f);
		char buff[64];
		sprintf(buff, "copy buffer %d", dua);
		RenderSystem->DrawText(buff, pos);

		pos.y -= 20.f;
		sprintf(buff, "state %d", state_);
		RenderSystem->DrawText(buff, pos);
	}


	virtual bool OnEvent(const Event& event) override
	{
		if (event.Type == Event::TouchDown)
		{
			if (state_ == 0)
			{
				texTest.Create(1024, 1024, RGBA8);
				state_++;
				return true;
			}

			if (state_ == 1)
			{
				renderTexture_.Create(1024, 1024, RGB8);
				Models[0]->texture_ = renderTexture_;

				GLog.LogInfo("%d * %d", RenderSystem->GetCurrentWindow().width,
					RenderSystem->GetCurrentWindow().height);	
				state_++;
				return true;
			}

			if (state_ == 2)
			{
				frameBuffer_.Create(1024, 1024, RGB8, 0);
				frameBuffer_.Bind();
				RenderSystem->BeginFrame();
				OnRender();
				RenderSystem->EndFrame();
				frameBuffer_.Unbind();
				checkGlError("render frame buffer");

				Models[0]->texture_ = frameBuffer_.GetTexture();
				state_++;
				return true;
			}
		}

		return true;
	}
	
	virtual void OnDestroy() override
	{
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
