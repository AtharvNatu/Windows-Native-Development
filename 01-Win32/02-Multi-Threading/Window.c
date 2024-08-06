#include <windows.h>
#include "Window.h"

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//* Global Thread Function Declrations
DWORD WINAPI ThreadProcIncrement(LPVOID);
DWORD WINAPI ThreadProcDecrement(LPVOID);

//* Entry-point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");

	//* Code
	ZeroMemory((void*)&wndclass, sizeof(WNDCLASSEX));
	
	//* Initialization of Window Class
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	//* Register the above created class
	RegisterClassEx(&wndclass);

	//* Get Screen Co-ordinates
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("Atharv Natu : Multi-Threading"),
		WS_OVERLAPPEDWINDOW,
		(screenX / 2) - (WINDOW_WIDTH / 2),
		(screenY / 2) - (WINDOW_HEIGHT / 2),
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	//* Message Loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Variable Declarations
	HANDLE hIncrementThread = NULL;
	HANDLE hDecrementThread = NULL;

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			
			//* Increment Thread
			hIncrementThread = CreateThread(
				NULL, //! Long Pointer to Security Attributes Structure
				0, //! Stack Size of thread (0 -> default stack size)
				(LPTHREAD_START_ROUTINE)ThreadProcIncrement, //! Register Thread Function's Name (Address)
				(LPVOID)hwnd, //! Parameter to be passed to thread function
				0, //! Thread Creation Flags (0 -> default behavior)
				NULL //! Thread Id (Legacy)
			);

			//* Decrement Thread
			hDecrementThread = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ThreadProcDecrement,
				(LPVOID)hwnd,
				0,
				NULL
			);

		break;

		case WM_LBUTTONDOWN:
			MessageBox(
				hwnd, 
				TEXT("MessageBox Thread"), 
				TEXT("Multi-Threading Demo"), 
				MB_ICONINFORMATION | MB_OK
			);
		break;

		case WM_DESTROY:

			if (hDecrementThread)
			{
				CloseHandle(hDecrementThread);
				hDecrementThread = NULL;
			}

			if (hIncrementThread)
			{
				CloseHandle(hIncrementThread);
				hIncrementThread = NULL;
			}

			PostQuitMessage(0);

		break;

		default:
		break;
	}

	//* Calling Default Window Procedure
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

DWORD WINAPI ThreadProcIncrement(LPVOID lpParam)
{
	// Variable Declarations
	HDC hdc = NULL;
	TCHAR str[255];

	// Code
	hdc = GetDC((HWND)lpParam);

	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(0, 255, 0));

	for (ULONG i = 0L; i <= ULONG_MAX; i++)
	{
		wsprintf(str, TEXT("Incrementing : %lu"), i);
		TextOut(hdc, 100, 50, str, wcslen(str));
	}

	if (hdc)
	{
		ReleaseDC((HWND)lpParam, hdc);
		hdc = NULL;
	}

	return 0;
}

DWORD WINAPI ThreadProcDecrement(LPVOID lpParam)
{
	// Variable Declrations
	HDC hdc = NULL;
	TCHAR str[255];

	hdc = GetDC((HWND)lpParam);

	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 0, 0));

	for (ULONG i = ULONG_MAX; i >= 0L; i--)
	{
		wsprintf(str, TEXT("Decrementing : %lu"), i);
		TextOut(hdc, 100, 100, str, wcslen(str));
	}

	if (hdc)
	{
		ReleaseDC((HWND)lpParam, hdc);
		hdc = NULL;
	}

	return 0;
}

