#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strsafe.h>
#include <tchar.h>
#include "Cheese.h"
#include "main.h"

// Global variables
BOOL IsWinVistaOrLater()
{
	// Initialize the OSVERSIONINFOEX structure.
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 0;

	// Initialize the condition mask.
	DWORDLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi,
		VER_MAJORVERSION | VER_MINORVERSION,
		dwlConditionMask);
}

NOTIFYICONDATA nid = {};

//static const UINT UWM_MOUSEMOVE;
POINT currPos;
POINT prevPos;
LONG vx;
LONG vy;
int x;
int y;
double angle = 45.0;
double rad = angle * PI / 180.0; // move these to update whenever the menu is updated
double cosA = cos(rad);
double sinA = sin(rad);

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Test Rotate");

HINSTANCE hInst;

// Forward declaration
//LRESULT WINAPI MouseProc(int, WPARAM, LPARAM);
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
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 100,
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
	// Initialize NotifyIconData (XP or above)
	IsWinVistaOrLater() ? nid.cbSize = sizeof(nid) : nid.cbSize = NOTIFYICONDATA_V3_SIZE;
	nid.hWnd = hWnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID;

	// {B937B6E9-2535-4D0E-9513-3A3F29CD26EC}
	static const GUID myGUID =
	{ 0xb937b6e9, 0x2535, 0x4d0e, { 0x95, 0x13, 0x3a, 0x3f, 0x29, 0xcd, 0x26, 0xec } };
	nid.guidItem = myGUID;

	//set icon's tooltip
	if FAILED(StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Test application")) {
		MessageBox(NULL,
			_T("Call to StringCchCopy for szTip failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}
	nid.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL;
	prevPos.x = -1;
	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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
	/*static HMENU hmenu;*/

	HDC hdc;            // handle to device context
	PAINTSTRUCT ps;
	TCHAR greeting[] = _T("Hello, Windows desktop!");
	TCHAR greeting2[] = _T("Hello, WinVista or later!");

	switch (message)
	{
	case WM_CREATE:

		// Save the menu handle
		//hmenu = GetMenu(hWnd);

		OutputDebugString(_T("create\n"));
		setHook(hWnd);
		//switch (LOWORD(wParam))
		//{
		//	// The user selected a hook command from the menu. 
		//case IDM_MOUSE:

		//	// Use the menu-item identifier as an index 
		//	// into the array of structures with hook data. 

		//	index = LOWORD(wParam);
		//	OutputDebugString(_T("Mouse hook\n"));

		//default:
		//	return (DefWindowProc(hWnd, message, wParam,
		//		lParam));
		//}
		break;
	case WH_MOUSE:
		OutputDebugString(_T("Msg received\n"));
		if (prevPos.x<0) {
			OutputDebugString(_T("1st\n"));
			GetCursorPos(&prevPos);
		}
		else {
			OutputDebugString(_T("gotcha\n"));
			GetCursorPos(&currPos);
			if (currPos.x == prevPos.x && currPos.y == prevPos.y) {
				break;
			}
			vx = currPos.x - prevPos.x; // vector coordinates, now rotate?
			vy = currPos.y - prevPos.y;
			
			x = prevPos.x + round(vx*cosA - vy*sinA);
			y = prevPos.y + round(sinA*vx + cosA*vy);

			SetCursorPos(x, y);
			wchar_t buf[1024];
			_snwprintf_s(buf, 1024, _TRUNCATE, L"prev x: %d, prev y: %d\n", prevPos.x, prevPos.y);
			OutputDebugString(buf);
			_snwprintf_s(buf, 1024, _TRUNCATE, L"curr x: %d, curr y: %d\n", currPos.x, currPos.y);
			OutputDebugString(buf);
			_snwprintf_s(buf, 1024, _TRUNCATE, L"new x: %d, new y: %d\n", x, y);
			OutputDebugString(buf);
			
			prevPos.x = x;
			prevPos.y = y;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		if (IsWinVistaOrLater())
		{
			TextOut(hdc,
				5, 5,
				greeting2, _tcslen(greeting2));
		} else
		{
			TextOut(hdc,
				5, 5,
				greeting, _tcslen(greeting));
		}
		EndPaint(hWnd, &ps);
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