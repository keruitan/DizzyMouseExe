#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <strsafe.h>
#include <tchar.h>

#pragma comment( lib, "user32.lib") 
#pragma comment( lib, "gdi32.lib")

#define NUMHOOKS 1
#define IDM_MOUSE 0
#define PI 3.14159265

// Global variables 

typedef struct _MYHOOKDATA
{
	int nType;
	HOOKPROC hkprc;
	HHOOK hhook;
} MYHOOKDATA;

MYHOOKDATA myhookdata[NUMHOOKS];
POINT cursorPos;
int xpos;
int ypos;
int vx;
int vy;
int angle = 45;

HWND gh_hwndMain;

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

HINSTANCE hInst;

// Hook procedures
LRESULT WINAPI MouseProc(int, WPARAM, LPARAM);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

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
	wcex.lpfnWndProc = MainWndProc;
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
	GetCursorPos(&cursorPos);
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


LRESULT WINAPI MainWndProc(HWND hwndMain, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL afHooks[NUMHOOKS];
	int index;
	static HMENU hmenu;

	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, Windows desktop!");

	gh_hwndMain = hwndMain;

	switch (uMsg)
	{
	case WM_CREATE:

		// Save the menu handle

		hmenu = GetMenu(hwndMain);

		// Initialize structures with hook data. The menu-item identifiers are 
		// defined as 0 through 6 in the header file app.h. They can be used to 
		// identify array elements both here and during the WM_COMMAND message. 

		myhookdata[IDM_MOUSE].nType = WH_MOUSE;
		myhookdata[IDM_MOUSE].hkprc = MouseProc;

		// Initialize all flags in the array to FALSE. 

		memset(afHooks, FALSE, sizeof(afHooks));
		OutputDebugString(_T("create/n"));
		OutputDebugString(_T("command/n"));
		switch (LOWORD(wParam))
		{
			// The user selected a hook command from the menu. 
		case IDM_MOUSE:

			// Use the menu-item identifier as an index 
			// into the array of structures with hook data. 

			index = LOWORD(wParam);
			OutputDebugString(_T("Mouse hook/n"));
			// If the selected type of hook procedure isn't 
			// installed yet, install it and check the 
			// associated menu item. 

			if (!afHooks[index])
			{
				myhookdata[index].hhook = SetWindowsHookEx(
					myhookdata[index].nType,
					myhookdata[index].hkprc,
					(HINSTANCE)NULL, GetCurrentThreadId());
				CheckMenuItem(hmenu, index,
					MF_BYCOMMAND | MF_CHECKED);
				afHooks[index] = TRUE;
			}

			// If the selected type of hook procedure is 
			// already installed, remove it and remove the 
			// check mark from the associated menu item. 

			else
			{
				UnhookWindowsHookEx(myhookdata[index].hhook);
				CheckMenuItem(hmenu, index,
					MF_BYCOMMAND | MF_UNCHECKED);
				afHooks[index] = FALSE;
			}

		default:
			return (DefWindowProc(hwndMain, uMsg, wParam,
				lParam));
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(gh_hwndMain, &ps);

		// Here your application is laid out.
		// For this introduction, we just print out "Hello, Windows desktop!"
		// in the top left corner.
		TextOut(hdc,
			5, 5,
			greeting, _tcslen(greeting));
		// End application-specific layout section.

		EndPaint(gh_hwndMain, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwndMain, uMsg, wParam, lParam);
	}
	return NULL;
}

/****************************************************************
WH_MOUSE hook procedure
****************************************************************/

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)  // do not process the message 
		return CallNextHookEx(myhookdata[IDM_MOUSE].hhook, nCode,
			wParam, lParam);
	xpos = LOWORD(lParam);
	ypos = HIWORD(lParam);

	vx = xpos - cursorPos.x;
	vy = ypos - cursorPos.y;

	SetCursorPos(cursorPos.x+(cos((angle*PI/180.0)*vx) - sin((angle*PI / 180.0)*vy)), cursorPos.y+(sin((angle*PI / 180.0)*vx) + cos((angle*PI / 180.0)*vy)) );
	GetCursorPos(&cursorPos);

	OutputDebugString(_T("In mouse proc/n"));

	return CallNextHookEx(myhookdata[IDM_MOUSE].hhook, nCode, wParam, lParam);
}