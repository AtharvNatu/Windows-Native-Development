#include "Window.h"

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//* Global Variable Declarations
HWND hwnd = NULL;
HRESULT hr = S_OK;

//! Direct2D Related
ID2D1Factory *pID2D1Factory = NULL;
IDWriteFactory *pIDWriteFactory = NULL;
ID2D1HwndRenderTarget *pID2D1HwndRenderTarget = NULL;
IDWriteTextFormat *pIDWriteTextFormat = NULL;
ID2D1SolidColorBrush *pID2D1SolidColorBrush = NULL;

//* Entry-point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Variable Declarations
	WNDCLASSEX wndclass;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");

	//* Code

	//! Start COM Engine
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("Failed To Initialize COM Library ... Exiting Now !!!"), TEXT("Image Editor"), MB_ICONERROR | MB_OK);
		exit(EXIT_FAILURE);
	}

	hr = CreateDeviceIndependentResources();
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("CreateDeviceIndependentResources() Failed ... Exiting Now !!!"), TEXT("Image Editor"), MB_ICONERROR | MB_OK);
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
	wndclass.hbrBackground = NULL;
	wndclass.hInstance = HINST_THISCOMPONENT;
	wndclass.hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hIconSm = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	//* Register the above created class
	RegisterClassEx(&wndclass);

	//! Create Application Window
	FLOAT dpiX, dpiY;
	pID2D1Factory->GetDesktopDpi(&dpiX, &dpiY);

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("Atharv Natu : Direct2D Hello World"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		NULL
	);

	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	//* Message Loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//! Stop COM Engine
	CoUninitialize();

	return (int)msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Variable Declarations
	HRESULT hrRender = S_OK;
	UINT width = 0, height = 0;

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_SIZE:

			width = LOWORD(lParam);
			height = HIWORD(lParam);

			if (pID2D1HwndRenderTarget)
			{
				D2D1_SIZE_U size;
				size.width = width;
				size.height = height;

				pID2D1HwndRenderTarget->Resize(size);
			}

		break;

		case WM_PAINT:
		case WM_DISPLAYCHANGE:
			
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			{
				hrRender = CreateDeviceResources();
				if (SUCCEEDED(hrRender) 
					&&
					!(pID2D1HwndRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
				{
					static const WCHAR szMsg[] = L"Hello, Direct2D !!!";

					//! Retrieve size of render target
					D2D1_SIZE_F renderTargetSize = pID2D1HwndRenderTarget->GetSize();
					pID2D1HwndRenderTarget->BeginDraw();
					{
						pID2D1HwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
						pID2D1HwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
						pID2D1HwndRenderTarget->DrawText(
							szMsg,
							ARRAYSIZE(szMsg) - 1,
							pIDWriteTextFormat,
							D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
							pID2D1SolidColorBrush
						);
					}
					hrRender = pID2D1HwndRenderTarget->EndDraw();
					if (hrRender == D2DERR_RECREATE_TARGET)
					{
						hrRender = S_OK;
						DiscardDeviceResources();
					}
				}
			}
			EndPaint(hwnd, &ps);
			
		break;


		case WM_DESTROY:
			SafeRelease(&pID2D1Factory);
			SafeRelease(&pIDWriteFactory);
			SafeRelease(&pID2D1HwndRenderTarget);
			SafeRelease(&pIDWriteTextFormat);
			SafeRelease(&pID2D1SolidColorBrush);

			PostQuitMessage(0);
		break;

		default:
		break;
	}

	//* Calling Default Window Procedure
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

HRESULT CreateDeviceIndependentResources()
{
	// Variable Declarations
	static const WCHAR szFontName[] = L"Verdana";
	static const FLOAT fontSize = 50;
	HRESULT hr = S_OK;

	// Code

	//! Create Direct2D Factory
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pID2D1Factory);
	if (SUCCEEDED(hr))
	{
		//! Create DirectWrite Factory
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pIDWriteFactory),
			reinterpret_cast<IUnknown**>(&pIDWriteFactory)
		);

		if (SUCCEEDED(hr))
		{
			//! Create DirectWrite Text Format Object
			hr = pIDWriteFactory->CreateTextFormat(
				szFontName,
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				fontSize,
				L"",
				&pIDWriteTextFormat
			);

			if (SUCCEEDED(hr))
			{
				//! Center Text Horizontally and Vertically
				pIDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				pIDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}
	}

	return hr;
}

HRESULT CreateDeviceResources()
{
	// Variable Declarations
	HRESULT hr = S_OK;

	// Code
	if (!pID2D1HwndRenderTarget)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		//! Create Direct2D Render Target
		hr = pID2D1Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, size),
			&pID2D1HwndRenderTarget
		);

		if (SUCCEEDED(hr))
			//! Create Black Brush
			hr = pID2D1HwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pID2D1SolidColorBrush);
	}

	return hr;
}

void DiscardDeviceResources()
{
	SafeRelease(&pID2D1HwndRenderTarget);
	SafeRelease(&pID2D1SolidColorBrush);
}





