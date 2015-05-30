#include <windows.h>
#include <string>
#include "glew.h"
#include "wglew.h"
#include "AuroraGL.h"
#include "MyLog.h"
#include "glUtil.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"opengl32.lib") 

glState_t glState;

HDC ghDC = NULL;

//
//bool InitGlewLib(HINSTANCE hInstance, HWND hWnd)
//{
//
//	HDC hDC = ::GetDC(hWnd);
//	HGLRC hRc;
//	::PIXELFORMATDESCRIPTOR pfd;
//	memset(&pfd,0,sizeof(pfd));
//
//	pfd.nVersion = 1;
//	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
//	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
//	pfd.iPixelType = PFD_TYPE_RGBA;
//	pfd.cColorBits = 32;
//	pfd.cDepthBits = 32;
//	pfd.iLayerType = PFD_MAIN_PLANE;       
//
//	int iPixelFormat = ::ChoosePixelFormat(hDC,&pfd);
//	if (iPixelFormat == 0)
//	{
//	    DestroyWindow(hWnd);
//	    return false;
//	}
//
//	if (::SetPixelFormat(hDC, iPixelFormat, &pfd))
//	{
//		hRc = ::wglCreateContext(hDC);
//		if (::wglMakeCurrent(hDC, hRc))
//		{
//			GLenum retVal = glewInit();
//			wglMakeCurrent(NULL, NULL);
//			wglDeleteContext(hRc);
//			DestroyWindow(hWnd);
//			return retVal == GLEW_OK;
//		}
//		wglDeleteContext(hRc);
//		DestroyWindow(hWnd);
//	}
//	else
//	{
//		DestroyWindow(hWnd);
//	}
//
//	return false;
//}

void GLAPIENTRY DebugCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	GLog.LogError("OpenGL Debug Message: SOURCE(0x%04X),"
		"TYPE(0x%04X),"
		"ID(0x%08X),"
		"SEVERITY(0x%04X), \"%s\"\n",
		source, type, id, severity, message);
}

bool InitOpenGL(HINSTANCE hInstance, HWND hWnd)
{	
	memset(&glState, 0, sizeof(glState));

	HDC hDC = ::GetDC(hWnd);

	::PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		32,                    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		24,                    // 32-bit z-buffer  
		8,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};


	HGLRC hRC = NULL;

	bool result = false;
	BOOL makeCurrent = FALSE;
	while (true)	{
		int iPixelFormat = ::ChoosePixelFormat(hDC, &pfd);
		if (iPixelFormat == 0)		{
			break;
		}

		if (!SetPixelFormat(hDC, iPixelFormat, &pfd))		{
			break;
		}
		
		hRC = ::wglCreateContext(hDC);
		if (!hDC)		{
			break;
		}

		makeCurrent = wglMakeCurrent(hDC, hRC);
		if (!makeCurrent)		{
			break;
		}

		if (glewInit() != GLEW_OK)		{
			break;
		}

		result = true;
		break;
	}

	if (makeCurrent)	{
		wglMakeCurrent(NULL, NULL);
	}

	if (hRC != NULL)	{
		wglDeleteContext(hRC);
	}

	if (!result)	{
		return false;
	}

	if (!WGLEW_ARB_create_context || !WGLEW_ARB_pixel_format)	{
		return false;
	}
	
	const int iPixelFormatAttributeList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,                        // 绘制到窗口
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,                            // 支持OpenGL
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,        // 硬件加速
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,                            // 双缓冲
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,                    // RGBA
		WGL_COLOR_BITS_ARB, 32,                                    // 颜色位数32
		WGL_DEPTH_BITS_ARB, 24,                                    // 深度位数24
		WGL_STENCIL_BITS_ARB, 8,                                    // 模板位数8
		WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,                // 双缓冲swap方式直接交换
		WGL_SAMPLES_ARB, 0,                                // 4倍抗锯齿
		0
	};

	const int iContextAttributeList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,                        // 主版本号
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,                        // 次版本号
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	int iPixelFormat, iNumFormat;
	if (!wglChoosePixelFormatARB(hDC, iPixelFormatAttributeList, NULL,
		1, &iPixelFormat, (UINT *)&iNumFormat))	{
		return false;
	}

	if (!SetPixelFormat(hDC, iPixelFormat, &pfd))	{
		int error = ::GetLastError();
		return false;
	}

	hRC = wglCreateContextAttribsARB(hDC, NULL, iContextAttributeList);
	if (!hRC){
		return false;
	}

	if (!wglMakeCurrent(hDC, hRC)) {
		wglDeleteContext(hRC);
		return false;
	}

	glState.hdc = hDC;

	RECT rect = { 0 };
	::GetClientRect(hWnd, &rect);
	glState.width	= rect.right - rect.left;
	glState.height	= rect.bottom - rect.top;

	LogGLInfo();


#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallbackFunction, NULL);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

	return true;
}
