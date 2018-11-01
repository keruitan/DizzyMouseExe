#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>
#include <tchar.h>

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

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Test Rotate");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
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

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
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

	HDC hdc;            // handle to device context 
	POINT pt;           // current cursor location 
	PAINTSTRUCT ps;
	TCHAR greeting[] = _T("Hello, Windows desktop!");
	TCHAR greeting2[] = _T("Hello, WinVista or later!");

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Here your application is laid out.
		// For this introduction, we just print out "Hello, Windows desktop!"
		// in the top left corner.
		if (IsWinVistaOrLater())
		{
			TextOut(hdc,
				5, 5,
				greeting2, _tcslen(greeting2));
		}
		else
		{
			TextOut(hdc,
				5, 5,
				greeting, _tcslen(greeting));
		}
		
		// End application-specific layout section.

		EndPaint(hWnd, &ps);
		break;

	case WM_MOUSEMOVE:
		// somehow rotate cursor here
		GetCursorPos(&pt);
		SetCursorPos(pt.x+1, pt.y);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}