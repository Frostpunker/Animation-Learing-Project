#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include<Windows.h>
#include"glad.h"
#include<iostream>
#include"Application.h"
#include"TestMain.h"

// Nuklear
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
extern "C" {
	NK_API nk_context* nk_win32_init();
	NK_API int nk_win32_handle_event(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, float scaleFactor);
	NK_API void nk_win32_render(int width, int height, int display_width, int display_height);
	NK_API void nk_blank_render(int width, int height, int display_width, int display_height);
	NK_API void nk_win32_shutdown(void);
	NK_API void nk_demo();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#if _DEBUG
#include <crtdbg.h>
#pragma comment( linker, "/subsystem:console" )
int main(int argc, const char** argv) {
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	return WinMain(GetModuleHandle(NULL), NULL,
		GetCommandLineA(), SW_SHOWDEFAULT);
}
#else
#pragma comment( linker, "/subsystem:windows" )
#endif
#pragma comment(lib, "opengl32.lib")

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char*
(WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

Application* gApplication = 0;
GLuint gVertexArrayObject = 0;
nk_context* gNkContext = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE
	hPrevInstance, PSTR szCmdLine,
	int iCmdShow)
{
	gApplication = new TestMain();
	const wchar_t CLASS_NAME[] = L"Win32 Game Window";

	//窗口程序的基本设置
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = CLASS_NAME;
	RegisterClassEx(&wndclass);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int clientWidth = 800;
	int clientHeight = 600;
	RECT windowRect;
	SetRect(&windowRect,
		(screenWidth / 2) - (clientWidth / 2),
		(screenHeight / 2) - (clientHeight / 2),
		(screenWidth / 2) + (clientWidth / 2),
		(screenHeight / 2) + (clientHeight / 2));

	//创建窗口
	DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // WS_THICKFRAME to resize
	AdjustWindowRectEx(&windowRect, style, FALSE, 0);
	HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, CLASS_NAME, style,
		windowRect.left, windowRect.top, windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top, NULL, NULL, hInstance, szCmdLine);
	HDC hdc = GetDC(hwnd);

	//创建OpenGL上下文
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW
		| PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);


	HGLRC tempRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC
		wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)
		wglGetProcAddress("wglCreateContextAttribsARB");

	//版本选择4.6
	const int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, };
	HGLRC hglrc = wglCreateContextAttribsARB(
		hdc, 0, attribList);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempRC);
	wglMakeCurrent(hdc, hglrc);

	if (!gladLoadGL()) {
		std::cout << "Could not initialize GLAD\n";
	}
	else {
		std::cout << "OpenGL Version " <<
			GLVersion.major << "." << GLVersion.minor <<
			"\n";
	}

	PFNWGLGETEXTENSIONSSTRINGEXTPROC
		_wglGetExtensionsStringEXT =
		(PFNWGLGETEXTENSIONSSTRINGEXTPROC)
		wglGetProcAddress("wglGetExtensionsStringEXT");
	bool swapControlSupported = strstr(
		_wglGetExtensionsStringEXT(),
		"WGL_EXT_swap_control") != 0;

	int vsynch = 0;
	if (swapControlSupported) {
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
			(PFNWGLSWAPINTERVALEXTPROC)
			wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC
			wglGetSwapIntervalEXT =
			(PFNWGLGETSWAPINTERVALEXTPROC)
			wglGetProcAddress("wglGetSwapIntervalEXT");
		if (wglSwapIntervalEXT(1)) 
		{
			std::cout << "Enabled vsynch\n";
			vsynch = wglGetSwapIntervalEXT();
		}
		else {
			std::cout << "Could not enable vsynch\n";
		}
	}
	else { // !swapControlSupported
		std::cout << "WGL_EXT_swap_control not supported\n";
	}

	glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	gApplication->Init();
	gNkContext = nk_win32_init();

	DWORD lastTick = GetTickCount64();
	MSG msg;
	while (true) {
		
		if (gNkContext != 0 && gVertexArrayObject != 0) {
			nk_input_begin(gNkContext);
		}
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (gNkContext != 0 && gVertexArrayObject != 0) {
			nk_input_end(gNkContext);
		}
		DWORD thisTick = GetTickCount64();
		float deltaTime = float(thisTick - lastTick) * 0.001f;
		lastTick = thisTick;
		//根据上一帧到当前帧的时间进行更新
		if (gApplication != 0) {
			gApplication->Update(deltaTime);
		}
		if (gApplication != 0) {
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			clientWidth = clientRect.right - clientRect.left;
			clientHeight = clientRect.bottom - clientRect.top;
			glViewport(0, 0, clientWidth, clientHeight);
			//启用深度测试
			glEnable(GL_DEPTH_TEST);
			//启用背面剔除
			glEnable(GL_CULL_FACE);
			glPointSize(5.0f);
			glBindVertexArray(gVertexArrayObject);

			glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			//计算屏幕宽高比用于渲染
			float aspect = (float)clientWidth / (float)clientHeight;
			gApplication->Render(aspect);
		}

		if (gApplication != 0) {
			gApplication->ImGui(gNkContext);
		}

		if (gNkContext != 0 && gVertexArrayObject != 0)
		{
			nk_win32_render((int)((float)clientWidth), 
				(int)((float)clientHeight), clientWidth, clientHeight);
		}
		if (gApplication != 0) {
			SwapBuffers(hdc);
			if (vsynch != 0) {
				glFinish();
			}
		}

	} // End of game loop

	if (gApplication != 0) {
		std::cout << "Expected application to be null on exit\n";
		delete gApplication;
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_CLOSE:
		if (gApplication != 0) {
			gApplication->ShutDown();
			gApplication = 0;
			DestroyWindow(hwnd);
		}
		else {
			std::cout << "Already shut down!\n";
		}
		break;
	case WM_DESTROY:
		if (gVertexArrayObject != 0) {
			HDC hdc = GetDC(hwnd);
			HGLRC hglrc = wglGetCurrentContext();

			glBindVertexArray(0);
			glDeleteVertexArrays(1, &gVertexArrayObject);
			gVertexArrayObject = 0;

			nk_win32_shutdown();
			gNkContext = 0;

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(hwnd, hdc);

			PostQuitMessage(0);
		}
		else {
			std::cout << "Got multiple destroy messages\n";
		}
		break;
	case WM_PAINT:
	case WM_ERASEBKGND:
		return 0;
	}
	if (gNkContext != 0 && gVertexArrayObject != 0) {
		if (nk_win32_handle_event(hwnd, iMsg, wParam, lParam, 1.0f)) {
			return 0;
		}
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}