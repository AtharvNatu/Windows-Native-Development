#include "../Include/Utils.h"

//* Global Variables
HINSTANCE ghInstance = NULL;
HRESULT hr = S_OK;
HBITMAP hBitmap = NULL, hOriginalBitmap = NULL;
HCURSOR hPickerCursor = NULL, hDefaultCursor = NULL;

IDesaturation *pIDesaturation = NULL;
ISepia *pISepia = NULL;
IColorInversion *pIColorInversion = NULL;

BOOL bDesaturate = FALSE;
BOOL bSepia = FALSE;
BOOL bInversion = FALSE;
BOOL bResetImage = FALSE;
BOOL bColorPick = FALSE;
BOOL bUserRegistered = FALSE;

FILE* gpFile_UserLog = NULL;
FILE* gpFile_AppLog = NULL;

FormattedTime formattedTime;

unsigned int giPixelX = 0, giPixelY = 0;

typedef struct tagUser
{
	char firstName[TEXT_LENGTH];
	char middleName[TEXT_LENGTH];
	char surname[TEXT_LENGTH];
} USER;

USER user;

//* Entry-point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
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

	ZeroMemory((void*)&wndclass, sizeof(WNDCLASSEX));
	
	//* Initialization of Window Class
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IE_MENU);
	wndclass.hbrBackground = CreateSolidBrush(RGB(197, 211, 224));
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ADN_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	//* Register the above created class
	RegisterClassEx(&wndclass);

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("Image Editor v1.0"),
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

	ghInstance = hInstance;

	ShowWindow(hwnd, iCmdShow);
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
	//* Variable Declarations
	RECT rc;
	PAINTSTRUCT ps;

	HDC hdc = NULL;

	COLORREF desaturatedPixelColor, sepiaPixelColor, negativePixelColor;

	static BOOL bImageLoaded = FALSE;
	static TCHAR szImagePath[_MAX_PATH];
	static unsigned int resizedWindowWidth = 0, resizedWindowHeight = 0;
	int errorStatus = 0;
	BOOL bCursorsLoaded = FALSE;
	
	//? User Registration
	int checkFileStatus = -1;
	char *lFileName = NULL;
	char fileName[] = "F:\\Win32-COM\\Windows-Native-Development\\03-Projects\\Image-Editor\\10-COM-Integration\\03-Client\\User-Log.log";
	
	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:

			ZeroMemory(&ps, sizeof(PAINTSTRUCT));

			//! Register Server Libraries
			if (!RegisterServerLibararies())
			{
				MessageBox(NULL, TEXT("Failed To Create Install Required Library Files ... Exiting Now !!!"), TEXT("Image Editor"), MB_ICONERROR | MB_OK);
				exit(EXIT_FAILURE);
			}

			//! App Log File
			if (!CreateOpenLogFile(&gpFile_AppLog, "ImageEditor.log", "a+"))
			{
				MessageBox(NULL, TEXT("Failed To Create App Log File ... Exiting Now !!!"), TEXT("Image Editor"), MB_ICONERROR | MB_OK);
				exit(EXIT_FAILURE);
			}

			//! Load Cursors
			bCursorsLoaded = LoadAppCursors(&hPickerCursor, &hDefaultCursor);
			if (!bCursorsLoaded)
				DestroyWindow(hwnd);

			//! COM Library
			hr = GetLibraryInterfaces(pIDesaturation, pISepia, pIColorInversion, &errorStatus);
			if (errorStatus < 0)
			{
				MessageBox(hwnd, TEXT("ImageEditor.dll and ImageToolkit.dll Not Found ... Exiting !!!"), TEXT("Image Editor"), MB_ICONERROR | MB_OK);
				if (DEBUG == 1)
					GetErrorMessage(hr, FALSE, NULL);
				formattedTime = GetFormattedTime();
				switch(errorStatus)
				{
					case -1: PrintLog(&gpFile_AppLog, "[%02d-%02d-%d %02d:%02d:%02d %s] Failed to obtain IDesaturation Interface !!!\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm); break;
					case -2: PrintLog(&gpFile_AppLog, "[%02d-%02d-%d %02d:%02d:%02d %s] Failed to obtain ISepia Interface !!!\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm); break;
					case -3: PrintLog(&gpFile_AppLog, "[%02d-%02d-%d %02d:%02d:%02d %s] Failed to obtain IColorInversion Interface !!!\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm); break;
					default: break;
				}
				DestroyWindow(hwnd); 
			}

		break;

		case WM_SIZE:
			resizedWindowWidth = LOWORD(lParam);
			resizedWindowHeight = HIWORD(lParam);
		break;

		case WM_PAINT:
			
			HDC hdcMem;
			BITMAP bitmap;
			HFONT hFont;

			GetClientRect(hwnd, &rc);
			hdc = BeginPaint(hwnd, &ps);
			{
				if (!bImageLoaded)
				{
					CreateAppFont(&hFont, TEXT("Poppins"), 36);
					HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
					SetBkColor(hdc, RGB(197, 211, 224));
					SetTextColor(hdc, RGB(85, 136, 198));
					DrawText(hdc, "Click On File Menu And Select 'Open' To Open An Image File", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					SelectObject(hdc, hOldFont);
					DeleteObject(hFont);
				}
				else
				{
					hdcMem = CreateCompatibleDC(hdc);
					if (!bResetImage)
					{
						GetObject(hOriginalBitmap, sizeof(BITMAP), &bitmap);
						SelectObject(hdcMem, hOriginalBitmap);
					}
					else
					{
						GetObject(hBitmap, sizeof(BITMAP), &bitmap);
						SelectObject(hdcMem, hBitmap);
					}
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
					
					if (bDesaturate)
					{
						for (int yRow = 0; yRow < resizedWindowHeight; yRow++)
						{
							for (int xColumn = 0; xColumn < resizedWindowWidth; xColumn++)
							{
								// Get color from the pixel at co-ordinate (X-Column,Y-Row)
								COLORREF originalPixelColor = GetPixel(hdc, xColumn, yRow);
								pIDesaturation->Desaturation(originalPixelColor, &desaturatedPixelColor);
								SetPixel(hdc, xColumn, yRow, desaturatedPixelColor);
							}
						}
					}

					if (bSepia)
					{
						for (int yRow = 0; yRow < resizedWindowHeight; yRow++)
						{
							for (int xColumn = 0; xColumn < resizedWindowWidth; xColumn++)
							{
								// Get color from the pixel at co-ordinate (X-Column,Y-Row)
								COLORREF originalPixelColor = GetPixel(hdc, xColumn, yRow);
								pISepia->Sepia(originalPixelColor, &sepiaPixelColor);
								SetPixel(hdc, xColumn, yRow, sepiaPixelColor);
							}
						}
					}

					if (bInversion)
					{
						for (int yRow = 0; yRow < resizedWindowHeight; yRow++)
						{
							for (int xColumn = 0; xColumn < resizedWindowWidth; xColumn++)
							{
								// Get color from the pixel at co-ordinate (X-Column,Y-Row)
								COLORREF originalPixelColor = GetPixel(hdc, xColumn, yRow);
								pIColorInversion->ColorInversion(originalPixelColor, &negativePixelColor);
								SetPixel(hdc, xColumn, yRow, negativePixelColor);
							}
						}
					}

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
			if (bImageLoaded)
			{
				unsigned int pickedPixelX = GET_X_LPARAM(lParam);
				unsigned int pickedPixelY = GET_Y_LPARAM(lParam);

				if (pickedPixelX >= 0 && pickedPixelX <= resizedWindowWidth && pickedPixelY >= 0 && pickedPixelY <= resizedWindowHeight)
					SetCursor(hPickerCursor);
				else
					SetCursor(hDefaultCursor);
			}
		break;

		case WM_SETFOCUS:

			lFileName = fileName;
			checkFileStatus = PathFileExists(lFileName);
			if (checkFileStatus == 1)
				bUserRegistered = TRUE;
			else
				bUserRegistered = FALSE;

			if (!bUserRegistered)
				DialogBox(ghInstance, MAKEINTRESOURCE(REGISTER_USER_DLG), hwnd, RegisterDialogProc);
				
		break;

		case WM_LBUTTONDOWN:
			if (bImageLoaded)
			{
				giPixelX = GET_X_LPARAM(lParam);
				giPixelY = GET_Y_LPARAM(lParam);
				bColorPick = TRUE;
			}
		break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDM_OPEN:
					
					OPENFILENAME ofn = OpenFileDialog(hwnd);
					if (GetOpenFileName((LPOPENFILENAME)&ofn))
					{
						wsprintf(szImagePath, ofn.lpstrFile);

						if (bImageLoaded)
						{
							DeleteImageObject(&hOriginalBitmap);
							DeleteImageObject(&hBitmap);
						}

						if (!LoadImageFromExplorer(&hBitmap, &hOriginalBitmap, szImagePath))
						{
							bImageLoaded = FALSE;
							MessageBox(NULL, TEXT("Failed To Load Bitmap ... Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
							DestroyWindow(hwnd);
						}

						bImageLoaded = TRUE;

						InvalidateRect(hwnd, NULL, TRUE);
					}

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

			SafeInterfaceRelease(pIDesaturation, pISepia, pIColorInversion);

			DeleteAppCursor(&hPickerCursor);
			DeleteImageObject(&hOriginalBitmap);
			DeleteImageObject(&hBitmap);

			CloseLogFile(&gpFile_UserLog);
			CloseLogFile(&gpFile_AppLog);

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
	HDC hdc = NULL, hdcPaint = NULL;
	HBRUSH hBrush = NULL;
	PAINTSTRUCT ps;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
			ZeroMemory(&ps, sizeof(PAINTSTRUCT));
		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(197, 211, 224));

		case WM_CTLCOLORSTATIC:
			hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(RGB(197, 211, 224));

		case WM_PAINT:
			hdcPaint = BeginPaint(hDlg, &ps);
			{
				if (bColorPick)
				{
					COLORREF pickedPixel = GetPixel(hdc, giPixelX, giPixelY);
					
					unsigned int rValue = GetRValue(pickedPixel);
					unsigned int gValue = GetGValue(pickedPixel);
					unsigned int bValue = GetBValue(pickedPixel);

					RECT colorRect = { 780, 150, 840, 210 };
					HBRUSH hBrush = CreateSolidBrush(RGB(rValue, gValue, bValue));
					FillRect(hdc, &colorRect, hBrush);
					DeleteObject(hBrush);

					bColorPick = FALSE;
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

				case ID_APPLY:
					if (IsDlgButtonChecked(hDlg, ID_RB_DESAT))
					{
						bDesaturate = TRUE;
						bSepia = FALSE;
						bInversion = FALSE;
					}
					else if (IsDlgButtonChecked(hDlg, ID_RB_SEPIA))
					{
						bDesaturate = FALSE;
						bSepia = TRUE;
						bInversion = FALSE;
					}
					else if (IsDlgButtonChecked(hDlg, ID_RB_INV))
					{
						bDesaturate = FALSE;
						bSepia = FALSE;
						bInversion = TRUE;
					}
					InvalidateRect(GetParent(hDlg), NULL, TRUE);
				break;

				case ID_RESET:
					bResetImage = TRUE;
					bDesaturate = FALSE;
					bSepia = FALSE;
					bInversion = FALSE;
					InvalidateRect(GetParent(hDlg), NULL, TRUE);
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
					
					GetDlgItemText(hDlg, ID_FNAME, user.firstName, TEXT_LENGTH);
					GetDlgItemText(hDlg, ID_MNAME, user.middleName, TEXT_LENGTH);
					GetDlgItemText(hDlg, ID_SNAME, user.surname, TEXT_LENGTH);
					
					//! Perform Data Validation


					if (!CreateOpenLogFile(&gpFile_UserLog, "User-Log.log", "w"))
					{
						MessageBox(NULL, TEXT("Failed To Create User Log File ... Exiting Now !!!"), TEXT("Image Editor Error"), MB_ICONERROR | MB_OK);
						exit(EXIT_FAILURE);
					}

					formattedTime = GetFormattedTime();

					PrintLog(&gpFile_UserLog, "Image Editor v1.0 User Registration Log\n");
					PrintLog(&gpFile_UserLog, "---------------------------------------------------------------\n");
					PrintLog(&gpFile_UserLog, "First Name : %s\n", user.firstName);
					PrintLog(&gpFile_UserLog, "Middle Name : %s\n", user.middleName);
					PrintLog(&gpFile_UserLog, "Surname : %s\n", user.surname);
					PrintLog(&gpFile_UserLog, "Date and Time : %02d-%02d-%d %02d:%02d:%02d %s\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm);
					PrintLog(&gpFile_UserLog, "---------------------------------------------------------------\n");

					bUserRegistered = TRUE;

					MessageBox(NULL, TEXT("User Registered Successfully ..."), TEXT("User Registration"), MB_ICONINFORMATION | MB_OK);

					EndDialog(hDlg, (INT_PTR)0);
				break;
			}
		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(197, 211, 224));

		case WM_CTLCOLORSTATIC:
			hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(RGB(197, 211, 224));

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





