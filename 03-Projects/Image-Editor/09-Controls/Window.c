#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include "Window.h"

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ControlsDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK RegisterDialogProc(HWND, UINT, WPARAM, LPARAM);

//* Global Function Declarations
OPENFILENAME OpenFileDialog(HWND hwndOwner);

//* Global Variables
HINSTANCE ghInstance = NULL;

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
	wndclass.lpszMenuName = MAKEINTRESOURCE(IE_MENU);
	wndclass.hbrBackground = CreateSolidBrush(RGB(244, 250, 255));
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	ghInstance = hInstance;

	//* Register the above created class
	RegisterClassEx(&wndclass);

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("Atharv Natu : Image Editor Controls"),
		WS_OVERLAPPEDWINDOW,
		0,
		0,
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

void ErrorExit(LPCTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//* Variable Declarations
	RECT rc;
	PAINTSTRUCT ps;

	HDC hdc = NULL;

	static HBITMAP hBitmap = NULL;
	static HCURSOR hPickerCursor = NULL, hDefaultCursor = NULL;
	static BOOL bImageLoaded = FALSE;
	static TCHAR szImagePath[_MAX_PATH];
	static unsigned int resizedWindowWidth = 0, resizedWindowHeight = 0;
	
	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			ZeroMemory(&ps, sizeof(PAINTSTRUCT));

			hPickerCursor = (HCURSOR)LoadImage(NULL, "ColorPicker.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			if (hPickerCursor == NULL)
			{
				MessageBox(NULL, TEXT("Failed To Load Picker Cursor .. Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
				ErrorExit(TEXT("LoadCursor"));
				DestroyWindow(hwnd);
			}
			

			hDefaultCursor = LoadCursor(NULL, IDC_ARROW);

		break;

		case WM_SIZE:
			resizedWindowWidth = LOWORD(lParam);
			resizedWindowHeight = HIWORD(lParam);
		break;

		case WM_PAINT:
			
			HDC hdcMem;
			BITMAP bitmap;

			GetClientRect(hwnd, &rc);
			hdc = BeginPaint(hwnd, &ps);
			{
				if (!bImageLoaded)
				{
					HFONT hFont = CreateFont(
						24,
						0,
						0,
						0,
						FW_MEDIUM,
						FALSE,
						FALSE,
						FALSE,
						ANSI_CHARSET,
						OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,
						DEFAULT_PITCH | FF_DONTCARE,
						TEXT("Tahoma")
					);

					HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
					SetBkColor(hdc, RGB(244, 250, 255));
					SetTextColor(hdc, RGB(137, 147, 255));
					DrawText(hdc, "Click On File Menu And Select 'Open' To Open An Image File", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					SelectObject(hdc, hOldFont);
					DeleteObject(hFont);
				}
				else
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
					if (hdcMem)
					{
						DeleteDC(hdcMem);
						hdcMem = NULL;
					}
				}
			}
			if (hdc)
			{
				EndPaint(hwnd, &ps);
				hdc = NULL;
			}
		break;

		case WM_MOUSEMOVE:
			//! For Color Picking
			unsigned int pickedPixelX = GET_X_LPARAM(lParam);
			unsigned int pickedPixelY = GET_Y_LPARAM(lParam);

			if (pickedPixelX >= 50 && pickedPixelX <= 150 && pickedPixelY >= 50 && pickedPixelY <= 150) {
                // Set the custom cursor
                SetCursor(hPickerCursor);
            } else {
                // Set the default cursor
                SetCursor(hDefaultCursor);
            }

		break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDM_OPEN:
					
					OPENFILENAME ofn = OpenFileDialog(hwnd);
					if (GetOpenFileName((LPOPENFILENAME)&ofn) == FALSE)
					{
						MessageBox(NULL, TEXT("Failed To Open File Dialog ... Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
						DestroyWindow(hwnd);
					}
					wsprintf(szImagePath, ofn.lpstrFile);

					if (bImageLoaded)
					{
						if (hBitmap)
						{
							DeleteObject(hBitmap);
							hBitmap = NULL;
						}
					}

					hBitmap = (HBITMAP)LoadImage(NULL, szImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
					if (hBitmap == NULL)
					{
						bImageLoaded = FALSE;
						MessageBox(NULL, TEXT("Failed To Load Bitmap ... Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
						DestroyWindow(hwnd);
					}
					bImageLoaded = TRUE;
					InvalidateRect(hwnd, NULL, TRUE);

				break;

				case IDM_EXIT:
					DestroyWindow(hwnd);
				break;

				case IDM_EDIT:
					DialogBox(ghInstance, MAKEINTRESOURCE(IE_DLG), hwnd, ControlsDialogProc);
				break;

				case IDM_ABOUT:
					DialogBox(ghInstance, MAKEINTRESOURCE(ABOUT_DLG), hwnd, AboutDialogProc);
				break;
			}
		break;

		case WM_DESTROY:

			if (hPickerCursor)
			{
				DestroyCursor(hPickerCursor);
				hPickerCursor = NULL;
			}
			
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


INT_PTR CALLBACK ControlsDialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Variable Declarations
	HDC hdc = NULL;
	HBRUSH hBrush = NULL;
	PAINTSTRUCT ps;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
			ZeroMemory(&ps, sizeof(PAINTSTRUCT));
		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(207, 241, 251));

		case WM_CTLCOLORSTATIC:
			hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(RGB(207, 241, 251));

		case WM_PAINT:
			HDC hdcPaint = BeginPaint(hDlg, &ps);
			{
				RECT colorRect = { 390, 440, 450, 500 };
				hBrush = CreateSolidBrush(RGB(203, 121, 251));
				FillRect(hdcPaint, &colorRect, hBrush);
				if (hBrush)
				{
					DeleteObject(hBrush);
					hBrush = NULL;
				}
			}
			if (hdcPaint)
			{
				EndPaint(hDlg, &ps);
				hdcPaint = NULL;
			}
		return (INT_PTR)TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_REGISTER:
					DialogBox(ghInstance, MAKEINTRESOURCE(REGISTER_USER_DLG), hDlg, RegisterDialogProc);
				break;

				case ID_ABOUT:
					DialogBox(ghInstance, MAKEINTRESOURCE(ABOUT_DLG), hDlg, AboutDialogProc);
				break;

				case ID_OK:
				case ID_EXIT:
					if (hdc)
					{
						hdc = NULL;
					}
					EndDialog(hDlg, (INT_PTR)0);
				break;
			}
		return (INT_PTR)TRUE;

		case WM_CLOSE:
			if (hdc)
			{
				hdc = NULL;
			}
			EndDialog(hDlg, (INT_PTR)0);
		return (INT_PTR)TRUE;

		default:
			return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//*Variable Declarations
	HDC hdc = NULL;
	static HBRUSH hBrush = NULL;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_ABT_OK:
					EndDialog(hDlg, (INT_PTR)0);
				break;
			}
		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
			return (INT_PTR)GetStockObject(BLACK_BRUSH);

		case WM_CTLCOLORSTATIC:
			hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(30, 144, 255));
			SetBkColor(hdc, RGB(0, 0, 0));
		return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));

		case WM_CLOSE:
			EndDialog(hDlg, (INT_PTR)0);
		break;

		default:
			return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK RegisterDialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//*Variable Declarations
	HDC hdc = NULL;
	static HBRUSH hBrush = NULL;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_REGISTER_BTN:
					
				break;
			}
		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(208, 221, 255));

		case WM_CTLCOLORSTATIC:
			hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(RGB(208, 221, 255));

		case WM_CLOSE:
			if (hdc)
			{
				hdc = NULL;
			}
			EndDialog(hDlg, (INT_PTR)0);
		break;

		default:
			return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}

OPENFILENAME OpenFileDialog(HWND hwndOwner)
{
	// Code
	OPENFILENAME ofn;
	TCHAR szFileName[_MAX_PATH];

	
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	szFileName[0] = '\0';

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndOwner;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "Bitmap Files (*.bmp)\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = TEXT("Select Image File");
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY | OFN_EXPLORER;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("*");
	ofn.lCustData = 0L;
	ofn.lpTemplateName = NULL;

	return ofn;
}
