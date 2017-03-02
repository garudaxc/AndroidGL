// OGLProject.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "resource.h"
#include "AuroraGL.h"
#include "Platfrom.h"
#include "Client.h"
#include "Rendering.h"
#include "Input.h"
#include "glUtil.h"

using namespace FancyTech;

#define MAX_LOADSTRING 100


// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


void _UpdateTimer();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OGLPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	Platfrom::Init();
	GInput->Create();
	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OGLPROJECT));

	bool bGotMsg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);
	while (WM_QUIT != msg.message)
	{
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);	

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			_UpdateTimer();
			MainClient->OnUpdate();

			RenderSystem->BeginFrame();
			MainClient->OnRender();
			RenderSystem->EndFrame();

			SwapBuffers(glState.hdc);
			checkGlError("SwapBuffers");
		}
	}

	//UnloadResource();
	Platfrom::Shutdown();
	return (int) msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OGLPROJECT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	   CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //if (!InitGlewLib(hInstance, hWnd))
   //{
	  // ::MessageBox(NULL, _T("glew init Error"), NULL, MB_OK);
	  // return 0;
   //}

   if (!InitOpenGL(hInstance, hWnd))
   {
	   ::MessageBox(NULL, _T("InitOpenGL Error"), NULL, MB_OK);
	   return 0;
   }

   if (MainClient == nullptr) {
	   ::MessageBox(NULL, _T("No valid client founded!"), NULL, MB_OK);
	   return FALSE;
   }

   RenderSystem->Create(glState.width, glState.height);
   MainClient->OnCreate();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//

namespace FancyTech
{
	// windows input
	void DispatchWindowsEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//PAINTSTRUCT ps;
	//HDC hdc;

	FancyTech::DispatchWindowsEvent(hWnd, message, wParam, lParam);
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	//case WM_PAINT:
	//	//hdc = BeginPaint(hWnd, &ps);
	//	//// TODO:  �ڴ���������ͼ����...
	//	//EndPaint(hWnd, &ps);
	//	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
