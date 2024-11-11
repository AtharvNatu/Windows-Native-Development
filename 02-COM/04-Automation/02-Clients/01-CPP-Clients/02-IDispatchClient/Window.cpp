#include <windows.h>
#include "Window.h"
#include "AutomationServer.h"

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//* Global Variable Declaration
IDispatch* pIDispatch = NULL;

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
		MessageBox(NULL, TEXT("Failed To Initialize COM Library ... Exiting Now !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
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
		TEXT("Atharv Natu : COM Automation C++ IDispatch Client"),
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
	// Function Declarations
	void SafeInterfaceRelease(void);
	void GetErrorMessage(HRESULT hr);

	// Variable Declarations
	HRESULT hr = S_OK;
	int iNum1 = 0, iNum2 = 0;
	TCHAR str[255];

	OLECHAR* szFunctionName1 = L"SumOfTwoIntegers";
	OLECHAR* szFunctionName2 = L"SubtractionOfTwoIntegers";

	DISPID dispid_SumOfTwoIntegers;
	DISPID dispid_SubtractionOfTwoIntegers;

	DISPPARAMS param;

	VARIANT vArg[2];
	VARIANT vRetval;
	
	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			
			hr = CoCreateInstance(
				CLSID_MyMath,
				NULL,						// NULL specifies No Aggregation
				CLSCTX_INPROC_SERVER,		// CLSCTX_INPROC_SERVER => Server Type is DLL
				IID_IDispatch,
				(void**)&pIDispatch
			);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain Implemented IDispatch Interface !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr);
				DestroyWindow(hwnd);
			}

			iNum1 = 55;
			iNum2 = 45;

			// Prepare the parameters to be passed to Invoke() which will be internally passed to the invoked function
			VariantInit(vArg);
			{
				vArg[0].vt = VT_INT;
				vArg[0].intVal = iNum2;
				
				vArg[1].vt = VT_INT;
				vArg[1].intVal = iNum1;

				param.rgvarg = vArg;
				param.cArgs = 2;
				param.cNamedArgs = 0;
				param.rgdispidNamedArgs = NULL;

				VariantInit(&vRetval);
				{
					// Get DISPID of SumOfTwoIntegers()
					hr = pIDispatch->GetIDsOfNames(
						IID_NULL,
						&szFunctionName1,
						1,
						GetUserDefaultLCID(),
						&dispid_SumOfTwoIntegers
					);
					if (FAILED(hr))
					{
						MessageBox(hwnd, TEXT("Failed To Obtain ID For SumOfTwoIntegers() !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
						GetErrorMessage(hr);
						DestroyWindow(hwnd);
					}

					hr = pIDispatch->Invoke(
						dispid_SumOfTwoIntegers,
						IID_NULL,
						GetUserDefaultLCID(),
						DISPATCH_METHOD,
						&param,
						&vRetval,
						NULL,
						NULL
					);
					if (FAILED(hr))
					{
						MessageBox(hwnd, TEXT("Failed To Invoke SumOfTwoIntegers() !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
						GetErrorMessage(hr);
						DestroyWindow(hwnd);
					}
					else
					{
						wsprintf(str, TEXT("Sum of %d and %d = %d"), iNum1, iNum2, vRetval.lVal);
						MessageBox(hwnd, str, TEXT("Addition Result"), MB_ICONINFORMATION | MB_OK);
					}

					// Get DISPID of SubtractionOfTwoIntegers()
					hr = pIDispatch->GetIDsOfNames(
						IID_NULL,
						&szFunctionName2,
						1,
						GetUserDefaultLCID(),
						&dispid_SubtractionOfTwoIntegers
					);
					if (FAILED(hr))
					{
						MessageBox(hwnd, TEXT("Failed To Obtain ID For SubtractionOfTwoIntegers() !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
						GetErrorMessage(hr);
						DestroyWindow(hwnd);
					}

					hr = pIDispatch->Invoke(
						dispid_SubtractionOfTwoIntegers,
						IID_NULL,
						GetUserDefaultLCID(),
						DISPATCH_METHOD,
						&param,
						&vRetval,
						NULL,
						NULL
					);
					if (FAILED(hr))
					{
						MessageBox(hwnd, TEXT("Failed To Invoke SubtractionOfTwoIntegers() !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
						GetErrorMessage(hr);
						DestroyWindow(hwnd);
					}
					else
					{
						wsprintf(str, TEXT("Subtraction of %d and %d = %d"), iNum1, iNum2, vRetval.lVal);
						MessageBox(hwnd, str, TEXT("Subtraction Result"), MB_ICONINFORMATION | MB_OK);
					}
				}
				VariantClear(&vRetval);
			}
			VariantClear(vArg);

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
	if (pIDispatch)
	{
		pIDispatch->Release();
		pIDispatch = NULL;
	}
}

