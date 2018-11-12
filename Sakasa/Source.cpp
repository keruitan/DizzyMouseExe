#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strsafe.h>
#include <tchar.h>
#include "Cheese.h"
#include "main.h"


POINT currPos;
POINT prevPos;
LONG vx;
LONG vy;
int x;
int y;
double angle = 15.0;
double rad = angle * PI / 180.0;
double cosA = cos(rad);
double sinA = sin(rad);

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Dizzy Mouse");

HINSTANCE hInst;

// Forward declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 0, 0: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	prevPos.x = -1;

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(
	HWND hWnd,          // window handle 
	UINT message,       // type of message 
	WPARAM wParam,        // additional information 
	LPARAM lParam)        // additional information 
{
	switch (message)
	{
	case WM_CREATE:
		setHook(hWnd);
		break;
	case WH_MOUSE:
		if (prevPos.x<0) {
			GetCursorPos(&prevPos);
		}
		else {
			GetCursorPos(&currPos);
			if (currPos.x == prevPos.x && currPos.y == prevPos.y) {
				break;
			}
			vx = currPos.x - prevPos.x; // vector coordinates, now rotate
			vy = currPos.y - prevPos.y;
			
			x = prevPos.x + round(vx*cosA - vy*sinA);
			y = prevPos.y + round(sinA*vx + cosA*vy);

			SetCursorPos(x, y);
			prevPos.x = x;
			prevPos.y = y;
		}
		break;
	case WM_DESTROY:
		unHook(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return NULL;
}