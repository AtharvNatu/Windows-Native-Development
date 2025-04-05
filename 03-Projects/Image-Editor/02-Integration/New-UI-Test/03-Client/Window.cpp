#include "Window.h"
#include "Server/ImageEditor.h"

//* Global Variables
struct User
{
	char firstName[TEXT_LENGTH];
	char middleName[TEXT_LENGTH];
	char lastName[TEXT_LENGTH];
} user;

//* Window Controls
HWND applyEffectButton = NULL, resetImageButton = NULL, destaurationRadioButton = NULL, sepiaRadioButton = NULL, negativeRadioButton = NULL;

HINSTANCE ghInstance = NULL;
HRESULT hr = S_OK;
HFONT hFont = NULL;

IDesaturation *pIDesaturation = NULL;
ISepia *pISepia = NULL;
IColorInversion *pIColorInversion = NULL;

BOOL bDesaturate = FALSE;
BOOL bSepia = FALSE;
BOOL bInversion = FALSE;
BOOL bResetImage = FALSE;
BOOL bColorPick = FALSE;

BOOL bUserValidation = FALSE;
BOOL bUserRegistered = FALSE;

FILE* gpFile_UserLog = NULL;

SYSTEMTIME systemTime;

unsigned int giPixelX = 0, giPixelY = 0;

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

	//! Start COM Engine
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("Failed To Initialize COM Library ... Exiting Now !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
		exit(EXIT_FAILURE);
	}
	
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
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghInstance = hInstance;

	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hwnd);

	AddWindowControls(hwnd);
	SetGlobalFont(hwnd);

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

void SetGlobalFont(HWND hwndParent)
{
	// Code
	hFont = CreateFont(
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
		PROOF_QUALITY,
		DEFAULT_PITCH | FF_MODERN,
		TEXT("Poppins")
	);

	SendMessage(applyEffectButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(resetImageButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(destaurationRadioButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(sepiaRadioButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(negativeRadioButton, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void AddWindowControls(HWND hwndParent)
{
	// Code
	applyEffectButton  = CreateWindow(
		TEXT("Button"),
		TEXT("Apply Effect"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		480,
		40,
		120,
		30,
		hwndParent,
		NULL,
		ghInstance,
		NULL
	);

	resetImageButton  = CreateWindow(
		TEXT("Button"),
		TEXT("Reset Image"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1700,
		80,
		120,
		30,
		hwndParent,
		NULL,
		ghInstance,
		NULL
	);

	destaurationRadioButton  = CreateWindowEx(
		0,
		TEXT("Button"),
		TEXT("Desaturation"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1700,
		250,
		120,
		30,
		hwndParent,
		NULL,
		ghInstance,
		NULL
	);

	sepiaRadioButton  = CreateWindowEx(
		0,
		TEXT("Button"),
		TEXT("Sepia"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1700,
		300,
		120,
		30,
		hwndParent,
		NULL,
		ghInstance,
		NULL
	);

	negativeRadioButton  = CreateWindowEx(
		0,
		TEXT("Button"),
		TEXT("Color Inversion"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1700,
		350,
		120,
		30,
		hwndParent,
		NULL,
		ghInstance,
		NULL
	);
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

	COLORREF desaturatedPixelColor, sepiaPixelColor, negativePixelColor;

	static HBITMAP hBitmap = NULL, hOriginalBitmap = NULL;
	static HCURSOR hPickerCursor = NULL, hDefaultCursor = NULL;
	static BOOL bImageLoaded = FALSE;
	static TCHAR szImagePath[_MAX_PATH];
	static unsigned int windowWidth = 0, windowHeight = 0;
	
	//? User Registration
	int checkFileStatus = -1;
	char *lFileName = NULL;
	char fileName[] = "F:\\Win32-COM\\Windows-Native-Development\\03-Projects\\Image-Editor\\01-Modules\\10-COM-Integration\\03-Client\\User-Log.log";

	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:
			ZeroMemory(&ps, sizeof(PAINTSTRUCT));

			hPickerCursor = (HCURSOR)LoadImage(NULL, TEXT("Assets/Images/ColorPicker.cur"), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			if (hPickerCursor == NULL)
			{
				MessageBox(NULL, TEXT("Failed To Load Picker Cursor .. Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
				ErrorExit(TEXT("LoadCursor"));
				DestroyWindow(hwnd);
			}
			
			hDefaultCursor = LoadCursor(NULL, IDC_ARROW);

			hr = CoCreateInstance(
				CLSID_ImageEditor,
				NULL,						
				CLSCTX_INPROC_SERVER,
				IID_Desaturation,
				(void**)&pIDesaturation
			);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain IDesaturation Interface !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr);
				DestroyWindow(hwnd);
			}

			//! Sepia
			hr = pIDesaturation->QueryInterface(IID_ISepia, (void**)&pISepia);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain ISepia Interface !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr);
				DestroyWindow(hwnd);
			}

			//! Color Inversion
			hr = pIDesaturation->QueryInterface(IID_IColorInversion, (void**)&pIColorInversion);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain IColorInversion Interface !!!"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr);
				DestroyWindow(hwnd);
			}

		break;

		// case WM_SIZE:
		// 	windowWidth = LOWORD(lParam);
		// 	windowHeight = HIWORD(lParam);
		// break;

		case WM_COMMAND:

			switch(LOWORD(wParam))
			{
				case IDM_OPEN:
					
					OPENFILENAME ofn = OpenImageFileDialog(hwnd);
					if (GetOpenFileName((LPOPENFILENAME)&ofn))
					{
						wsprintf(szImagePath, ofn.lpstrFile);

						if (bImageLoaded)
						{
							if (hOriginalBitmap)
							{
								DeleteObject(hOriginalBitmap);
								hOriginalBitmap = NULL;
							}

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

						hOriginalBitmap = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
						if (hOriginalBitmap == NULL)
						{
							bImageLoaded = FALSE;
							MessageBox(NULL, TEXT("Failed To Store Bitmap ... Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
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

				// case ID_RB_DESAT:
				// 	if (HIWORD(wParam) == BN_CLICKED)
				// 	{
				// 		bDesaturate = TRUE;
				// 		bSepia = FALSE;
				// 		bInversion = FALSE;
				// 	}
				// break;

				// case ID_APPLY:
				// 	BOOL destaurationClicked = SendMessage(destaurationRadioButton, BM_GETCHECK, 0, 0) == BST_CHECKED;
				// 	InvalidateRect(hwnd, NULL, TRUE);
				// break;
			}
		break;

		case WM_PAINT:
			
			HDC hdcMem;
			BITMAP bitmap;

			GetClientRect(hwnd, &rc);
			windowWidth = rc.right;
        	windowHeight = rc.bottom;

			hdc = BeginPaint(hwnd, &ps);
			{
				//! Create Borders
				HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
				HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

				// Draw the border of the panel
				Rectangle(hdc, rc.right - 250, rc.top + 5, rc.right - 5, rc.bottom - 20);

				// Clean up
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);

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

				//! Image Scaling
				int canvasX = (windowWidth - CANVAS_WIDTH) / 2;
				int canvasY = (windowHeight - CANVAS_HEIGHT) / 2;

				RECT canvas = 
				{
					canvasX,
					canvasY,
					canvasX + CANVAS_WIDTH,
					canvasY + CANVAS_HEIGHT
				};

				HBRUSH canvasBrush = CreateSolidBrush(RGB(255, 255, 255));
				FillRect(hdc, &canvas, canvasBrush);
				DeleteObject(canvasBrush);

				int imageWidth = bitmap.bmWidth;
				int imageHeight = bitmap.bmHeight;

				float scaleX = (float)CANVAS_WIDTH / (float)imageWidth;
				float scaleY = (float)CANVAS_HEIGHT / (float)imageHeight;
				float scaleFactor = min(scaleX, scaleY);

				int newWidth = (int)(imageWidth * scaleFactor);
				int newHeight = (int)(imageHeight * scaleFactor);

				int xPosition = canvasX + (CANVAS_WIDTH - newWidth) / 2;
				int yPosition = canvasY + (CANVAS_HEIGHT - newHeight) / 2;

				StretchBlt(
					hdc,
					xPosition,
					yPosition,
					newWidth,
					newHeight,
					hdcMem,
					0,
					0,
					imageWidth,
					imageHeight,
					SRCCOPY
				);

				if (bDesaturate)
				{
					for (int yRow = 0; yRow < newHeight; yRow++)
					{
						for (int xColumn = 0; xColumn < newWidth; xColumn++)
						{
							// Get color from the pixel at co-ordinate (X-Column,Y-Row)
							int scaledX = (int)(xColumn / scaleFactor);
							int scaledY = (int)(yRow / scaleFactor);

							COLORREF originalPixelColor = GetPixel(hdc, scaledX, scaledY);
							pIDesaturation->Desaturation(originalPixelColor, &desaturatedPixelColor);
							SetPixel(hdc, scaledX, scaledY, desaturatedPixelColor);
						}
					}
				}

				if (bSepia)
				{
					for (int yRow = 0; yRow < windowHeight; yRow++)
					{
						for (int xColumn = 0; xColumn < windowWidth; xColumn++)
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
					for (int yRow = 0; yRow < windowHeight; yRow++)
					{
						for (int xColumn = 0; xColumn < windowWidth; xColumn++)
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

				if (pickedPixelX >= 0 && pickedPixelX <= windowWidth && pickedPixelY >= 0 && pickedPixelY <= windowHeight)
					SetCursor(hPickerCursor);
				else
					SetCursor(hDefaultCursor);
			}
		break;

		case WM_SETFOCUS:

			// lFileName = fileName;
			// checkFileStatus = PathFileExists(lFileName);
			// if (checkFileStatus == 1)
			// 	bUserRegistered = TRUE;
			// else
			// 	bUserRegistered = FALSE;

			// if (!bUserRegistered)
			// 	DialogBox(ghInstance, MAKEINTRESOURCE(REGISTER_USER_DLG), hwnd, RegisterDialogProc);
				
		break;

		case WM_LBUTTONDOWN:
			if (bImageLoaded)
			{
				giPixelX = GET_X_LPARAM(lParam);
				giPixelY = GET_Y_LPARAM(lParam);
				bColorPick = TRUE;
			}
		break;

		case WM_DESTROY:

			SafeInterfaceRelease();

			if (hPickerCursor)
			{
				DestroyCursor(hPickerCursor);
				hPickerCursor = NULL;
			}

			if (hOriginalBitmap)
			{
				DeleteObject(hOriginalBitmap);
				hOriginalBitmap = NULL;
			}
			
			if (hBitmap)
			{
				DeleteObject(hBitmap);
				hBitmap = NULL;
			}

			if (gpFile_UserLog)
			{
				fclose(gpFile_UserLog);
				gpFile_UserLog = NULL;
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
					GetDlgItemText(hDlg, ID_LNAME, user.lastName, TEXT_LENGTH);
					
					//! Perform Data Validation
					// ValidateUserData(user.firstName);
					// ValidateUserData(user.middleName);
					// ValidateUserData(user.lastName);

					gpFile_UserLog = fopen("User-Log.log", "w");
					if (gpFile_UserLog == NULL)
					{
						MessageBox(NULL, TEXT("Failed To Create User Log File !!!"), TEXT("Error"), MB_ICONERROR | MB_OK);
						return (INT_PTR)FALSE;
					}

					GetLocalTime(&systemTime);
					
					fprintf(gpFile_UserLog, "Image Editor v1.0 User Registration Log\n");
					fprintf(gpFile_UserLog, "---------------------------------------------------------------\n");
					fprintf(gpFile_UserLog, "First Name : %s\n", user.firstName);
					fprintf(gpFile_UserLog, "Middle Name : %s\n", user.middleName);
					fprintf(gpFile_UserLog, "Surname : %s\n", user.lastName);
					fprintf(gpFile_UserLog, "Date and Time : %02d:%02d\n", systemTime.wHour, systemTime.wMinute);
					fprintf(gpFile_UserLog, "---------------------------------------------------------------\n");

					bUserRegistered = TRUE;

					MessageBox(NULL, TEXT("User Registered Successfully ..."), TEXT("User Registration"), MB_ICONINFORMATION | MB_OK);

					fclose(gpFile_UserLog);

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

			// if (lParam == (LPARAM)GetDlgItem(hDlg, ID_FNAME_LBL) || 
			// 	lParam == (LPARAM)GetDlgItem(hDlg, ID_MNAME_LBL) || 
			// 	lParam == (LPARAM)GetDlgItem(hDlg, ID_LNAME_LBL))
			// {
			// 	if (bUserValidation)
				
			// 		SetTextColor(hdc, RGB(255, 0, 0));
			// 	else
			// 		SetTextColor(hdc, RGB(0, 0, 0));
			// }

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

OPENFILENAME OpenImageFileDialog(HWND hwndOwner)
{
	// Code
	OPENFILENAME ofn;
	TCHAR szFileName[_MAX_PATH];

	
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	szFileName[0] = '\0';

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndOwner;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = TEXT("Bitmap Files\0*.bmp\0");
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
	ofn.lpstrDefExt = TEXT("bmp");
	ofn.lCustData = 0L;
	ofn.lpTemplateName = NULL;

	return ofn;
}

void ValidateUserData(char *szData)
{
	// Code
	for (int i = 0; szData[i] != '\0'; i++)
	{
		if (isalpha(szData[i]) != 0)
			bUserValidation = FALSE;
	}

	bUserValidation = TRUE;
}

void GetErrorMessage(HRESULT hr)
{
	// Variable Declarationss
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
	if (pIColorInversion)
	{
		pIColorInversion->Release();
		pIColorInversion = NULL;
	}

	if (pISepia)
	{
		pISepia->Release();
		pISepia = NULL;
	}

	if (pIDesaturation)
	{
		pIDesaturation->Release();
		pIDesaturation = NULL;
	}
}

