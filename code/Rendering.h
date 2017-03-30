#include "Model.h"
#include "Texture.h"
#include "AurMath.h"

using namespace std;

#define COLOR(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)

namespace FancyTech
{
	
	class ModelInstance
	{
	public:
		Model		mesh_;
		Texture		texture_;
		Matrix4f	transform_;

	private: 
	};

	ModelInstance*	CreateModel(const char* mesh, const char* texture);


	struct ViewParam
	{
		Vector4f	clearColor;
		float		clearDepth;
		Matrix4f	ViewMatrix;
		Matrix4f	ProjMatrix;
	};

	struct RenderWindow
	{
		int		width;
		int		height;
	};

	class IRenderSystem
	{
	public:
		virtual void Create(int width, int height) = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void BeginView(const ViewParam& view) = 0;
		virtual void DrawElement() = 0;

		virtual void DrawText(const char* text, const Vector3f& pos) = 0;

		virtual void Destroy() = 0;

		virtual const RenderWindow& GetCurrentWindow() = 0;
	};

	extern IRenderSystem* RenderSystem;


}