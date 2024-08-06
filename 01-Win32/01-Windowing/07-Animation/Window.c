#include <windows.h>
#include "Window.h"

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
		TEXT("Atharv Natu : Animation"),
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

	static PAINTSTRUCT ps;
	static RECT rect;

	static int leftTopX, leftTopY, rightBottomX, rightBottomY, xAnimate = 0;
	static BOOL bTransition = TRUE;

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			ZeroMemory((void*)&ps, sizeof(PAINTSTRUCT));
			ZeroMemory((void*)&rect, sizeof(rect));
			SetTimer(hwnd, ADN_TIMER, 1, NULL);
		break;

		case WM_PAINT:
			
			GetClientRect(hwnd, &rect);
			hdc = BeginPaint(hwnd, &ps);
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			SelectObject(hdc, hBrush);
			
			leftTopX = 0 + xAnimate;
			leftTopY = (WINDOW_HEIGHT / 2) - (ELLIPSE_BOX_HEIGHT / 2);
			rightBottomX = ELLIPSE_BOX_HEIGHT + xAnimate;
			rightBottomY = (WINDOW_HEIGHT / 2) + (ELLIPSE_BOX_HEIGHT / 2);
			Ellipse(hdc, leftTopX, leftTopY, rightBottomX, rightBottomY);

			if (hBrush)
			{
				DeleteObject(hBrush);
				hBrush = NULL;
			}

			if (hdc)
			{
				EndPaint(hwnd, &ps);
				hdc = NULL;
			}

		break;

		case WM_TIMER:
			KillTimer(hwnd, ADN_TIMER);
			if (bTransition)
			{
				xAnimate++;
				if (rightBottomX == WINDOW_WIDTH)
					bTransition = FALSE;
			}
			else
			{
				xAnimate--;
				if (leftTopX == 0)
					bTransition = TRUE;
			}
			SetTimer(hwnd, ADN_TIMER, 1, NULL);
			InvalidateRect(hwnd, NULL, TRUE);
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


