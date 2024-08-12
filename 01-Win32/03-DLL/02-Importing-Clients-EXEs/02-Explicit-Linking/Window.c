#include <windows.h>
#include "Window.h"

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("Atharv Natu : Explicit Linking"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
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
	int num = 21;
	int cube = 0;
	TCHAR str[256];

	HMODULE hDll = NULL;

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			
			//* Load DLL Explicitly
			hDll = LoadLibrary(TEXT("MyMathTwo.dll"));
			if (hDll == NULL)
			{
				MessageBox(NULL, TEXT("LoadLibrary() Failed !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
				DestroyWindow(hwnd);
			}

			typedef int (*MakeCubeFnPtr)(int);
			MakeCubeFnPtr fnPtr = NULL;

			fnPtr = (MakeCubeFnPtr)GetProcAddress(hDll, "MakeCube");
			if (fnPtr == NULL)
			{
				MessageBox(NULL, TEXT("GetProcAddress() Failed !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
				FreeLibrary(hDll);
				hDll = NULL;
				DestroyWindow(hwnd);
			}

			cube = fnPtr(num);
			wsprintf(str, TEXT("Cube of %d = %d"), num, cube);
			MessageBox(hwnd, str, TEXT("Explicit Linking"), MB_OK | MB_ICONINFORMATION);

			FreeLibrary(hDll);
			hDll = NULL;

			DestroyWindow(hwnd);

		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;

		default:
		break;
	}

	//* Calling Default Window Procedure
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}


