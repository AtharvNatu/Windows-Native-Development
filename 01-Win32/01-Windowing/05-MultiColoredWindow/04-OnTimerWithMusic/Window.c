#include <windows.h>
//? #include <mmsystem.h>	-> For PlaySound(), now added in windows.h
#include "Window.h"

//* Import Libraries
#pragma comment(lib, "winmm.lib")

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

	//* Get Screen Co-ordinates
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("Atharv Natu : Multi-Colored Window on Timer with Music"),
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
	HDC hdc = NULL;
	HBRUSH hBrush = NULL;
	enum WindowColor
	{
		GRAY,
		RED,
		GREEN,
		BLUE,
		CYAN,
		MAGENTA,
		YELLOW,
		WHITE,
		BLACK
	};

	static PAINTSTRUCT ps;
	static RECT rect;
	static enum WindowColor eWindowColor = GRAY;

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			ZeroMemory((void*)&ps, sizeof(PAINTSTRUCT));
			ZeroMemory((void*)&rect, sizeof(rect));

			//* Play Music
			PlaySound(
				MAKEINTRESOURCE(ADN_MUSIC), 
				GetModuleHandle(NULL),
				SND_ASYNC | SND_RESOURCE | SND_LOOP
			);

			SetTimer(hwnd, ADN_TIMER, 1000, NULL);
		break;

		case WM_PAINT:
			GetClientRect(hwnd, &rect);
			hdc = BeginPaint(hwnd, &ps);
			{	 
				switch(eWindowColor)
				{
					case RED:
						hBrush = CreateSolidBrush(RGB(255, 0, 0));
					break;

					case BLUE:
						hBrush = CreateSolidBrush(RGB(0, 0, 255));
					break;

					case GREEN:
						hBrush = CreateSolidBrush(RGB(0, 255, 0));
					break;

					case CYAN:
						hBrush = CreateSolidBrush(RGB(0, 255, 255));
					break;

					case MAGENTA:
						hBrush = CreateSolidBrush(RGB(255, 0, 255));
					break;

					case YELLOW:
						hBrush = CreateSolidBrush(RGB(255, 255, 0));
					break;

					case WHITE:
						hBrush = CreateSolidBrush(RGB(255, 255, 255));
					break;

					case GRAY:
						hBrush = CreateSolidBrush(RGB(128, 128, 128));
					break;

					case BLACK:
						hBrush = CreateSolidBrush(RGB(0, 0, 0));
					break;

					default:
						hBrush = CreateSolidBrush(RGB(128, 128, 128));
					break;
				}
					
				SelectObject(hdc, hBrush);
				FillRect(hdc, &rect, hBrush);

				if (hBrush)
				{
					DeleteObject(hBrush);
					hBrush = NULL;
				}
			}
			if (hdc)
			{
				EndPaint(hwnd, &ps);
				hdc = NULL;
			}
		break;

		case WM_TIMER:
			KillTimer(hwnd, ADN_TIMER);
			eWindowColor++;
			if (eWindowColor > 8)
				eWindowColor = GRAY;
			SetTimer(hwnd, ADN_TIMER, 1000, NULL);
			InvalidateRect(hwnd, NULL, TRUE);
		break;

		case WM_DESTROY:
			//* Stop Music
			PlaySound(NULL, 0, 0);
			PostQuitMessage(0);
		break;

		default:
		break;
	}

	//* Calling Default Window Procedure
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}


