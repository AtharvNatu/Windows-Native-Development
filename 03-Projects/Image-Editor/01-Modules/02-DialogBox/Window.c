#include <windows.h>
#include "Window.h"

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ControlsDialogProc(HWND, UINT, WPARAM, LPARAM);

//* Global Variable Declarations
BOOL bOkPressed = FALSE;
BOOL bCancelPressed = FALSE;

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
		TEXT("Atharv Natu : Dialog Box"),
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//*Variable Declarations
	HINSTANCE hInst = NULL;
	HDC hdc = NULL;

	PAINTSTRUCT ps;
	RECT rc;

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			ZeroMemory(&ps, sizeof(PAINTSTRUCT));
			hInst = (HINSTANCE)((LPCREATESTRUCT)lParam)->hInstance;
		break;

		case WM_PAINT:
			GetClientRect(hwnd, &rc);
			hdc = BeginPaint(hwnd, &ps);
			{
				SetBkColor(hdc, RGB(0, 0, 0));

				if (bOkPressed)
				{
					SetTextColor(hdc, RGB(0, 255, 0));
					DrawText(hdc, "Ok Button Pressed", -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}

				if (bCancelPressed)
				{
					SetTextColor(hdc, RGB(255, 0, 0));
					DrawText(hdc, "Cancel Button Pressed", -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
				
			}
			if (hdc)
			{
				EndPaint(hwnd, &ps);
				hdc = NULL;
			}
		break;

		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				case VK_SPACE:
					DialogBox(hInst, MAKEINTRESOURCE(IE_DLG), hwnd, ControlsDialogProc);
				break;
			}
		break;

		case WM_DESTROY:
			hInst = NULL;
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
	//*Variable Declarations
	HWND hwndParent;
	RECT rc;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					bOkPressed = TRUE;
					bCancelPressed = FALSE;
					InvalidateRect(GetParent(hDlg), NULL, TRUE);
				break;

				case IDCANCEL:
					bCancelPressed = TRUE;
					bOkPressed = FALSE;
					InvalidateRect(GetParent(hDlg), NULL, TRUE);
				break;
			}
		return (INT_PTR)TRUE;

		case WM_CLOSE:
			EndDialog(hDlg, (INT_PTR)0);
		break;

		default:
			return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}


