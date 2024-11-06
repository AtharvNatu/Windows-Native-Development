#include <windows.h>
#include "Window.h"
#include "AutomationServer.h"

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
IMyMath* pIMyMath = NULL;

//* Entry-point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	HRESULT hr = S_OK;

	//* Code

	// Start COM Engine
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("Failed to initialize COM Library ... Exiting Now !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
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
		TEXT("Atharv Natu : COM Automation C++ IClassFactory Client"),
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

	// Stop COM Engine
	CoUninitialize();

	return (int)msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Variable Declarations
	HRESULT hr = S_OK;
	int iNum1 = 0, iNum2 = 0, iResult = 0;
	TCHAR str[255];

	// Function Declarations
	void SafeInterfaceRelease(void);
	void GetErrorMessage(HRESULT hr);

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			
			hr = CoCreateInstance(
				CLSID_MyMath,
				NULL,						// NULL specifies No Aggregation
				CLSCTX_INPROC_SERVER,		// CLSCTX_INPROC_SERVER => Server Type is DLL
				IID_IMyMath,
				(void**)&pIMyMath
			);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain IMyMath Interface !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr);
				DestroyWindow(hwnd);
			}

			iNum1 = 55;
			iNum2 = 45;

			// Addition
			pIMyMath->SumOfTwoIntegers(iNum1, iNum2, &iResult);
			wsprintf(str, TEXT("Sum of %d and %d = %d"), iNum1, iNum2, iResult);
			MessageBox(hwnd, str, TEXT("Addition Result"), MB_ICONINFORMATION | MB_OK);

			// Subtraction
			pIMyMath->SubtractionOfTwoIntegers(iNum1, iNum2, &iResult);
			wsprintf(str, TEXT("Subtraction of %d and %d = %d"), iNum1, iNum2, iResult);
			MessageBox(hwnd, str, TEXT("Subtraction Result"), MB_ICONINFORMATION | MB_OK);

			DestroyWindow(hwnd);

		break;

		case WM_DESTROY:
			SafeInterfaceRelease();
			PostQuitMessage(0);
		break;

		default:
		break;
	}

	//* Calling Default Window Procedure
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void GetErrorMessage(HRESULT hr)
{
	// Variable Declarations
	LPVOID buffer;

	// Code
	DWORD dw = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&buffer,
		0,
		NULL
	);

	if (dw != 0)
	{
		MessageBox(NULL, (LPCTSTR)buffer, TEXT("COM Error"), MB_ICONERROR | MB_OK);
		LocalFree(buffer);
	}
	else
		MessageBox(NULL, TEXT("Unknown Error Code !!!"), TEXT("Unknown Error"), MB_ICONERROR | MB_OK);
}

void SafeInterfaceRelease(void)
{
	// Code
	if (pIMyMath)
	{
		pIMyMath->Release();
		pIMyMath = NULL;
	}
}

