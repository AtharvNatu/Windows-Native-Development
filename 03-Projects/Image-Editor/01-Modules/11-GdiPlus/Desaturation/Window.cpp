#include <windows.h>
#include <stdio.h>
#include <gdiplus.h>
#include "Window.h"

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//* Global Variable Declarations
FILE* gpFile_UserLog = NULL;
ULONG_PTR gdiplusToken;
Gdiplus::Bitmap *gBitmap = nullptr;

//* Entry-point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");

	//* Code

	//! Initialize GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (status != Gdiplus::Ok)
	{
		MessageBox(NULL, TEXT("Failed to initialize GDI+ !!!"), TEXT("Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

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
		TEXT("Atharv Natu : GDI+"),
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
	HDC hdc = NULL;

	static HBITMAP hBitmap = NULL;
	static PAINTSTRUCT ps;

	static unsigned int resizedWindowWidth = 0, resizedWindowHeight = 0;
	static BOOL bSpaceKeyPressed = FALSE;

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			ZeroMemory((void**)&ps, sizeof(PAINTSTRUCT));

			hBitmap = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(ADN_BITMAP));
			if (hBitmap == NULL)
			{
				MessageBox(hwnd, TEXT("Failed To Load Bitmap ... Exiting !!!"), TEXT("Bitmap Error"), MB_ICONERROR | MB_OK);
				DestroyWindow(hwnd);
			}

			//! Convert to GDI+ Bitmap
			gBitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, nullptr);
			if (!gdiplusBitmap)
			{
				MessageBox(hwnd, TEXT("Failed To Create GDI+ Bitmap ... Exiting !!!"), TEXT("Bitmap Error"), MB_ICONERROR | MB_OK);
				DeleteObject(hBitmap);
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
					0, 
					0, 
					resizedWindowWidth,
					resizedWindowHeight,
					hdcMem, 
					0, 
					0, 
					bitmap.bmWidth,
					bitmap.bmHeight,
					SRCCOPY
				);

				// Pixel-by-pixel desaturation
				if (bSpaceKeyPressed)
				{
					for (int yRow = 0; yRow < resizedWindowHeight; yRow++)
					{
						for (int xColumn = 0; xColumn < resizedWindowWidth; xColumn++)
						{
							// Get color from the pixel at co-ordinate (X-Column,Y-Row)
							COLORREF originalPixelColor = GetPixel(hdc, xColumn, yRow);

							unsigned int originalR = GetRValue(originalPixelColor);
							unsigned int originalG = GetGValue(originalPixelColor);
							unsigned int originalB = GetBValue(originalPixelColor);

							unsigned int desaturatedR = (unsigned int)((float)originalR * 0.3f);
							unsigned int desaturatedG = (unsigned int)((float)originalG * 0.59f);
							unsigned int desaturatedB = (unsigned int)((float)originalB * 0.11f);

							unsigned int finalDesaturatedColor = desaturatedR + desaturatedG + desaturatedB;
							COLORREF desaturatedPixelColor = RGB(finalDesaturatedColor, finalDesaturatedColor, finalDesaturatedColor);

							SetPixel(hdc, xColumn, yRow, desaturatedPixelColor);
						}
					}
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

		case WM_SIZE:
			resizedWindowWidth = LOWORD(lParam);
			resizedWindowHeight = HIWORD(lParam);
		break;

		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
				case VK_SPACE:
					bSpaceKeyPressed = TRUE;
					InvalidateRect(hwnd, NULL, TRUE);
				break;

				default:
				break;
			}
		break;

		case WM_DESTROY:
			if (hBitmap)
			{
				DeleteObject(hBitmap);
				hBitmap = NULL;
			}
			PostQuitMessage(0);
		break;

		default:
		break;
	}

	//* Calling Default Window Procedure
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}


