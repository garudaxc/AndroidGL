#include <windows.h>
#include "glew.h"
#include "wglew.h"


struct glState_t
{
	int		width;
	int		height;
	HDC		hdc;
};

extern glState_t glState;

bool InitOpenGL(HINSTANCE hInstance, HWND hWnd);

void DrawFrame();

void LoadResource();
void UnloadResource();