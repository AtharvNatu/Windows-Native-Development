// #include "Window.h"

// #define WINDOW_WIDTH 	800
// #define WINDOW_HEIGHT	600

// //* Global Callback Declaration
// LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// HRESULT hr = S_OK;

// //! D2D Related
// HWND hwnd = NULL;
// ID2D1Factory *pID2D1Factory = NULL;
// IWICImagingFactory *pIWICImagingFactory = NULL;
// IDWriteFactory *pIDWriteFactory = NULL;
// ID2D1HwndRenderTarget *pID2D1HwndRenderTarget = NULL;
// IDWriteTextFormat *pIDWriteTextFormat = NULL;
// ID2D1SolidColorBrush *pID2D1SolidColorBrush = NULL;

// //* Entry-point Function
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
// {
// 	// Variable Declarations
// 	WNDCLASSEX wndclass;
// 	HWND hwnd;
// 	MSG msg;
// 	TCHAR szAppName[] = TEXT("MyWindow");

// 	//* Code

// 	//! Start COM Engine
// 	hr = CoInitialize(NULL);
// 	if (FAILED(hr))
// 	{
// 		MessageBox(NULL, TEXT("Failed To Initialize COM Library ... Exiting Now !!!"), TEXT("Image Editor"), MB_ICONERROR | MB_OK);
// 		exit(EXIT_FAILURE);
// 	}

// 	hr = CreateDeviceIndependentResources();
// 	if (FAILED(hr))
// 	{
// 		MessageBox(NULL, TEXT("CreateDeviceIndependentResources() Failed ... Exiting Now !!!"), TEXT("Image Editor"), MB_ICONERROR | MB_OK);
// 		exit(EXIT_FAILURE);
// 	}

// 	ZeroMemory((void*)&wndclass, sizeof(WNDCLASSEX));
	
// 	//* Initialization of Window Class
// 	wndclass.cbSize = sizeof(WNDCLASSEX);
// 	wndclass.style = CS_HREDRAW | CS_VREDRAW;
// 	wndclass.cbClsExtra = 0;
// 	wndclass.cbWndExtra = 0;
// 	wndclass.lpfnWndProc = WndProc;
// 	wndclass.lpszClassName = szAppName;
// 	wndclass.lpszMenuName = NULL;
// 	wndclass.hbrBackground = NULL;
// 	wndclass.hInstance = HINST_THISCOMPONENT;
// 	wndclass.hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(ADN_ICON));
// 	wndclass.hIconSm = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(ADN_ICON));
// 	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

// 	//* Register the above created class
// 	RegisterClassEx(&wndclass);

// 	//! Create Application Window
// 	FLOAT dpiX, dpiY;
// 	pID2D1Factory->GetDesktopDpi(&dpiX, &dpiY);

// 	//* Create Window in Memory
// 	hwnd = CreateWindow(
// 		szAppName,
// 		TEXT("Atharv Natu : Direct2D Hello World"),
// 		WS_OVERLAPPEDWINDOW,
// 		CW_USEDEFAULT,
// 		CW_USEDEFAULT,
// 		static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
// 		static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
// 		NULL,
// 		NULL,
// 		HINST_THISCOMPONENT,
// 		NULL
// 	);

// 	ShowWindow(hwnd, SW_SHOWNORMAL);
// 	UpdateWindow(hwnd);

// 	//* Message Loop
// 	while (GetMessage(&msg, NULL, 0, 0))
// 	{
// 		TranslateMessage(&msg);
// 		DispatchMessage(&msg);
// 	}

// 	//! Stop COM Engine
// 	CoUninitialize();

// 	return (int)msg.wParam;

// }

// LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
// {
// 	// Variable Declarations
// 	HRESULT hrRender = S_OK;
// 	UINT width = 0, height = 0;

// 	// Code
// 	switch (iMsg)
// 	{
// 		//* Message Handlers
// 		case WM_SIZE:

// 			width = LOWORD(lParam);
// 			height = HIWORD(lParam);

// 			if (pID2D1HwndRenderTarget)
// 			{
// 				D2D1_SIZE_U size;
// 				size.width = width;
// 				size.height = height;

// 				pID2D1HwndRenderTarget->Resize(size);
// 			}

// 		break;

// 		case WM_PAINT:
// 		case WM_DISPLAYCHANGE:
			
// 			PAINTSTRUCT ps;
// 			BeginPaint(hwnd, &ps);
// 			{
// 				hrRender = CreateDeviceResources();
// 				if (SUCCEEDED(hrRender) 
// 					&&
// 					!(pID2D1HwndRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
// 				{
// 					static const WCHAR szMsg[] = L"Hello, Direct2D !!!";

// 					//! Retrieve size of render target
// 					D2D1_SIZE_F renderTargetSize = pID2D1HwndRenderTarget->GetSize();
// 					pID2D1HwndRenderTarget->BeginDraw();
// 					{
// 						pID2D1HwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
// 						pID2D1HwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
// 						pID2D1HwndRenderTarget->DrawText(
// 							szMsg,
// 							ARRAYSIZE(szMsg) - 1,
// 							pIDWriteTextFormat,
// 							D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
// 							pID2D1SolidColorBrush
// 						);
// 					}
// 					hrRender = pID2D1HwndRenderTarget->EndDraw();
// 					if (hrRender == D2DERR_RECREATE_TARGET)
// 					{
// 						hrRender = S_OK;
// 						DiscardDeviceResources();
// 					}
// 				}
// 			}
// 			EndPaint(hwnd, &ps);
			
// 		break;


// 		case WM_DESTROY:
// 			SafeRelease(&pID2D1Factory);
// 			SafeRelease(&pIDWriteFactory);
// 			SafeRelease(&pID2D1HwndRenderTarget);
// 			SafeRelease(&pIDWriteTextFormat);
// 			SafeRelease(&pID2D1SolidColorBrush);

// 			PostQuitMessage(0);
// 		break;

// 		default:
// 		break;
// 	}

// 	//* Calling Default Window Procedure
// 	return DefWindowProc(hwnd, iMsg, wParam, lParam);
// }

// HRESULT CreateDeviceIndependentResources()
// {
// 	// Variable Declarations
// 	static const WCHAR szFontName[] = L"Verdana";
// 	static const FLOAT fontSize = 50;
// 	HRESULT hr = S_OK;
// 	ID2D1GeometrySink *pID2D1GeometrySink = NULL;

// 	// Code

// 	//! Create Direct2D Factory
// 	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pID2D1Factory);
// 	if (SUCCEEDED(hr))
// 	{
// 		//! Create DirectWrite Factory
// 		hr = DWriteCreateFactory(
// 			DWRITE_FACTORY_TYPE_SHARED,
// 			__uuidof(pIDWriteFactory),
// 			reinterpret_cast<IUnknown**>(&pIDWriteFactory)
// 		);

// 		if (SUCCEEDED(hr))
// 		{
// 			//! Create DirectWrite Text Format Object
// 			hr = pIDWriteFactory->CreateTextFormat(
// 				szFontName,
// 				NULL,
// 				DWRITE_FONT_WEIGHT_NORMAL,
// 				DWRITE_FONT_STYLE_NORMAL,
// 				DWRITE_FONT_STRETCH_NORMAL,
// 				fontSize,
// 				L"",
// 				&pIDWriteTextFormat
// 			);

// 			if (SUCCEEDED(hr))
// 			{
// 				//! Center Text Horizontally and Vertically
// 				pIDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
// 				pIDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
// 			}
// 		}
// 	}

// 	SafeRelease(&pID2D1GeometrySink);
// 	return hr;
// }

// HRESULT CreateDeviceResources()
// {
// 	// Variable Declarations
// 	HRESULT hr = S_OK;

// 	// Code
// 	if (!pID2D1HwndRenderTarget)
// 	{
// 		RECT rc;
// 		GetClientRect(hwnd, &rc);

// 		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

// 		//! Create Direct2D Render Target
// 		hr = pID2D1Factory->CreateHwndRenderTarget(
// 			D2D1::RenderTargetProperties(),
// 			D2D1::HwndRenderTargetProperties(hwnd, size),
// 			&pID2D1HwndRenderTarget
// 		);

// 		if (SUCCEEDED(hr))
// 		{
// 			//! Create Black Brush
// 			hr = pID2D1HwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pID2D1SolidColorBrush);
// 		}
// 	}

// 	return hr;
// }

// void DiscardDeviceResources()
// {
// 	SafeRelease(&pID2D1HwndRenderTarget);
// 	SafeRelease(&pID2D1SolidColorBrush);
// }













// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#include "Window.h"


//
// Provides the entry point to the application.
//
int WINAPI WinMain(
    HINSTANCE /*hInstance*/,
    HINSTANCE /*hPrevInstance*/,
    LPSTR /*lpCmdLine*/,
    int /*nCmdShow*/
    )
{
    // Ignore the return value because we want to run the program even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}


//
// Initialize members.
//
DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pDWriteFactory(NULL),
    m_pRenderTarget(NULL),
    m_pTextFormat(NULL),
    m_pBlackBrush(NULL)
{
}

//
// Release resources.
//
DemoApp::~DemoApp()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pTextFormat);
    SafeRelease(&m_pBlackBrush);

}

//
// Creates the application window and initializes
// device-independent resources.
//
HRESULT DemoApp::Initialize()
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = DemoApp::WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo Application",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);

            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}


//
// Create resources which are not bound
// to any device. Their lifetime effectively extends for the
// duration of the app. These resources include the Direct2D and
// DirectWrite factories,  and a DirectWrite Text Format object
// (used for identifying particular font characteristics).
//
HRESULT DemoApp::CreateDeviceIndependentResources()
{
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 50;
    HRESULT hr;
    ID2D1GeometrySink *pSink = NULL;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDWriteFactory),
            reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &m_pTextFormat
            );
    }
    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    }

    SafeRelease(&pSink);

    return hr;
}

//
//  This method creates resources which are bound to a particular
//  Direct3D device. It's all centralized here, in case the resources
//  need to be recreated in case of Direct3D device loss (eg. display
//  change, remoting, removal of video card, etc).
//
HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {

        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );
        if (SUCCEEDED(hr))
        {
            // Create a black brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBlackBrush
                );
        }

    }

    return hr;
}


//
//  Discard device-specific resources which need to be recreated
//  when a Direct3D device is lost
//
void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBlackBrush);
}

//
// The main window message loop.
//
void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


//
//  Called whenever the application needs to display the client
//  window. This method writes "Hello, World"
//
//  Note that this function will not render anything if the window
//  is occluded (e.g. when the screen is locked).
//  Also, this function will automatically discard device-specific
//  resources if the Direct3D device disappears during function
//  invocation, and will recreate the resources the next time it's
//  invoked.
//
HRESULT DemoApp::OnRender()
{
    HRESULT hr;

    hr = CreateDeviceResources();

    if (SUCCEEDED(hr) && !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
        static const WCHAR sc_helloWorld[] = L"Hello, World!";

        // Retrieve the size of the render target.
        D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        m_pRenderTarget->DrawText(
            sc_helloWorld,
            ARRAYSIZE(sc_helloWorld) - 1,
            m_pTextFormat,
            D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
            m_pBlackBrush
            );

        hr = m_pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
    }

    return hr;
}

//
//  If the application receives a WM_SIZE message, this method
//  resizes the render target appropriately.
//
void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        // Note: This method can fail, but it's okay to ignore the
        // error here -- it will be repeated on the next call to
        // EndDraw.
        m_pRenderTarget->Resize(size);
    }
}


//
// The window message handler.
//
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                ));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                wasHandled = true;
                result = 0;
                break;

            case WM_PAINT:
            case WM_DISPLAYCHANGE:
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hwnd, &ps);

                    pDemoApp->OnRender();
                    EndPaint(hwnd, &ps);
                }
                wasHandled = true;
                result = 0;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                wasHandled = true;
                result = 1;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}
