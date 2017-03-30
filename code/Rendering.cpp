#include "Rendering.h"
#include "BitmapFont.h"
#include "SpriteBatch.h"
#include "FileSystem.h"
#include "ShaderManager.h"
#include "GlobalVar.h"
#include "glUtil.h"
#include "Timer.h"

namespace FancyTech
{
	
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


	class RenderSystemLocal : public IRenderSystem
	{
	public:
		RenderSystemLocal()
		{
		}

		~RenderSystemLocal()
		{
		}

		virtual void Create(int width, int height) override
		{
			mainWindow_.width = width;
			mainWindow_.height = height;

			LogGLInfo();

			GGlobalVarManager->Init();

			bitmapFont.LoadFromFile("consolas.bitmapfont");
			spriteBatch.Init(128);
		}

		virtual void BeginView(const ViewParam& view) override
		{
			Vector4f color = view.clearColor;
			glClearColor(color.r, color.g, color.b, color.a);

			glClearDepthf(view.clearDepth);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//glDisable(GL_CULL_FACE);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);

			GShaderManager.Bind(ShaderDiffuse);
			GShaderManager.SetUnifrom(SU_VIEW, view.ViewMatrix);
			GShaderManager.SetUnifrom(SU_PROJECTION, view.ProjMatrix);
		}

		virtual void DrawElement() override
		{
		}

		virtual void Destroy() override
		{
		}

		virtual void BeginFrame() override
		{
		}

		virtual void EndFrame() override
		{
			float lineHeight = -30.0f;
			Vector3f pos(10.f, mainWindow_.height - 10.f, 0.f);

			char buff[64];
			sprintf(buff, "fps %.2f", Time.GetFPS());
			bitmapFont.DrawString(&spriteBatch, buff, pos);
			spriteBatch.Commit(mainWindow_.width, mainWindow_.height);

			glFlush();
			glFinish();
		}

		virtual const RenderWindow& GetCurrentWindow() override
		{
			return mainWindow_;
		}

		virtual void DrawText(const char* text, const Vector3f& pos) override
		{
			bitmapFont.DrawString(&spriteBatch, text, pos);
		}

	private:
		RenderWindow  mainWindow_;

	};

	static RenderSystemLocal renderSystemLocal;
	IRenderSystem* RenderSystem = &renderSystemLocal;

}