#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "Window.h"

#define WINDOW_WIDTH 	1024
#define WINDOW_HEIGHT	768

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//* Global Variable Declrations
FILE *gpFile_UserLog = NULL, *gpFile_ColorPickerLog = NULL, *gpFile_OGL_Log = NULL;

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
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	//* Register the above created class
	RegisterClassEx(&wndclass);

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("Atharv Natu : Color Picker"),
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
	//* Function Declarations
	BOOL createAndOpenLogFiles();
	BOOL closeLogFiles();
	BOOL checkSelectedPixel(int x, int y, RECT *rect);

	//* Variable Declarations
	HDC hdc = NULL;

	static HBITMAP hBitmap = NULL;
	static PAINTSTRUCT ps;

	static unsigned int resizedWindowWidth = 0, resizedWindowHeight = 0;
	static int rectWidth = 960, rectHeight = 540;
	static unsigned int pickedPixelX = 0, pickedPixelY = 0;
	static BOOL bColorPickerLeftClick = FALSE;

	TCHAR rgbValues[255];
	RECT imageRect = { 25, 25, 25 + rectWidth, 25 + rectHeight };

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			
			if (createAndOpenLogFiles() == FALSE)
				DestroyWindow(hwnd);

			ZeroMemory((void**)&ps, sizeof(PAINTSTRUCT));

			hBitmap = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(ADN_BITMAP));
			if (hBitmap == NULL)
			{
				MessageBox(hwnd, TEXT("Failed To Load Bitmap ... Exiting !!!"), TEXT("Bitmap Error"), MB_ICONERROR | MB_OK);
				DestroyWindow(hwnd);
			}

		break;


		case WM_PAINT:
			
			HDC hdcMem;
			BITMAP bitmap;

			hdc = BeginPaint(hwnd, &ps);
			{
				hdcMem = CreateCompatibleDC(hdc);
				GetObject(hBitmap, sizeof(BITMAP), &bitmap);
				SelectObject(hdcMem, hBitmap);

				SetStretchBltMode(hdc, COLORONCOLOR);
				StretchBlt(
					hdc, 
					imageRect.left, 
					imageRect.top, 
					imageRect.right - imageRect.left,
					imageRect.bottom - imageRect.top,
					hdcMem, 
					0, 
					0, 
					bitmap.bmWidth,
					bitmap.bmHeight,
					SRCCOPY
				);

				if (bColorPickerLeftClick)
				{
					COLORREF pickedPixel = GetPixel(hdc, pickedPixelX, pickedPixelY);
					
					unsigned int rValue = GetRValue(pickedPixel);
					unsigned int gValue = GetGValue(pickedPixel);
					unsigned int bValue = GetBValue(pickedPixel);

					// fprintf(gpFile_ColorPickerLog, "R : %u, G : %u, B : %u\n", rValue, gValue, bValue);

					RECT colorRect = { 450, 580, 530, 660 };
					HBRUSH hBrush = CreateSolidBrush(RGB(rValue, gValue, bValue));
					FillRect(hdc, &colorRect, hBrush);

					wsprintf(rgbValues, "R : %u, G : %u, B : %u\n", rValue, gValue, bValue);
					SetBkColor(hdc, RGB(0, 0, 0));
					SetTextColor(hdc, RGB(0, 255, 0));

					SetTextColor(hdc, RGB(0, 0, 0)); 
					SetBkMode(hdc, TRANSPARENT);
					TextOut(hdc, 420, 680, rgbValues, strlen(rgbValues));

					bColorPickerLeftClick = FALSE;
				}
				
				if (hdcMem)
				{
					DeleteDC(hdcMem);
					hdcMem = NULL;
				}
				
			}
			if (hdc)
			{
				EndPaint(hwnd, &ps);
				hdc = NULL;
			}
			
		break;

		case WM_LBUTTONDOWN:
			
			//! For Color Picking
			pickedPixelX = GET_X_LPARAM(lParam);
			pickedPixelY = GET_Y_LPARAM(lParam);

			if (checkSelectedPixel(pickedPixelX, pickedPixelY, &imageRect))
			{
				bColorPickerLeftClick = TRUE;
				InvalidateRect(hwnd, NULL, TRUE);
			}

		break;

		case WM_SIZE:
			resizedWindowWidth = LOWORD(lParam);
			resizedWindowHeight = HIWORD(lParam);
		break;

		case WM_DESTROY:
			
			if (hBitmap)
			{
				DeleteObject(hBitmap);
				hBitmap = NULL;
			}

			closeLogFiles();

			PostQuitMessage(0);

		break;

		default:
		break;
	}

	//* Calling Default Window Procedure
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

BOOL checkSelectedPixel(int x, int y, RECT *rect)
{
	// Code
	return (x >= rect->left && x <= rect->right && y >= rect->top && y <= rect->bottom);
}

BOOL createAndOpenLogFiles(void)
{
	//* Code
	gpFile_UserLog = fopen("User-Log.log", "w");
	if (gpFile_UserLog == NULL)
	{
		MessageBox(NULL, TEXT("Failed To Create User Log File ... Exiting !!!"), TEXT("Error"), MB_ICONERROR | MB_OK);
		return FALSE;
	}
	fprintf(gpFile_UserLog, "Program Started Successfully ...\n");
	fprintf(gpFile_UserLog, "User Log File Created Successfully ...\n");

	gpFile_ColorPickerLog = fopen("Color-Picker-Log.log", "w");
	if (gpFile_ColorPickerLog == NULL)
	{
		MessageBox(NULL, TEXT("Failed To Create Color Picker Log File ... Exiting !!!"), TEXT("Error"), MB_ICONERROR | MB_OK);
		return FALSE;
	}

	gpFile_OGL_Log = fopen("OGL-Color.log", "w");
	if (gpFile_ColorPickerLog == NULL)
	{
		MessageBox(NULL, TEXT("Failed To Create Color Picker Log File ... Exiting !!!"), TEXT("Error"), MB_ICONERROR | MB_OK);
		return FALSE;
	}

	return TRUE;

}

BOOL closeLogFiles(void)
{
	//* Code
	if (gpFile_OGL_Log)
	{
		fclose(gpFile_OGL_Log);
		gpFile_OGL_Log = NULL;
	}

	if (gpFile_ColorPickerLog)
	{
		fclose(gpFile_ColorPickerLog);
		gpFile_ColorPickerLog = NULL;
	}

	if (gpFile_UserLog)
	{
		fprintf(gpFile_UserLog, "Program Terminated Successfully ...\n");
		fclose(gpFile_UserLog);
		gpFile_UserLog = NULL;
	}

	return TRUE;

}


