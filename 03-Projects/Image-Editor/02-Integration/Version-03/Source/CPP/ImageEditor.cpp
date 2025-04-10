#include "Utils.h"
#include "ImageEffects.cuh"

//* Global Variables
HWND hwndControlsDialog = NULL;
HWND hwndProgressDialog = NULL;
HINSTANCE ghInstance = NULL;
HRESULT hr = S_OK;
HMENU hMenu = NULL;
HBITMAP hBitmap = NULL;
HCURSOR hPickerCursor = NULL, hDefaultCursor = NULL;

cv::Mat ocvImage, renderImage;
COLORREF desaturatedPixelColor, sepiaPixelColor, negativePixelColor;

IDesaturation *pIDesaturation = NULL;
ISepia *pISepia = NULL;
IColorInversion *pIColorInversion = NULL;

BOOL bColorPick = FALSE;
BOOL bUserRegistered = FALSE;
BOOL bValidateFirstName = FALSE;
BOOL bValidateMiddleName = FALSE;
BOOL bValidateLastName = FALSE;
BOOL bImageLoaded = FALSE;

FILE* gpFile_UserLog = NULL;
FILE* gpFile_ColorPickerLog = NULL;
FILE* gpFile_NormalizedColorPickerLog = NULL;
FILE* gpFile_AppLog = NULL;

FormattedTime formattedTime;

unsigned int giPixelX = 0, giPixelY = 0;

USER user;
RGB rgb;

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
		MessageBox(NULL, TEXT("Failed To Initialize COM Library ... Exiting Now !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
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
	wndclass.hbrBackground = CreateSolidBrush(BLUE_BG);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IE_APP_ICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IE_APP_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	//* Register the above created class
	RegisterClassEx(&wndclass);

	//* Create Window in Memory
	hwnd = CreateWindow(
		szAppName,
		TEXT("PhotoMind v1.0"),
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
		if (hwndControlsDialog == NULL || (IsDialogMessage(hwndControlsDialog, &msg) == 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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

	void *lpBits = nullptr;

	static TCHAR szImagePath[_MAX_PATH];
	static unsigned int resizedWindowWidth = 0, resizedWindowHeight = 0;
	int errorStatus = 0;
	BOOL bCursorsLoaded = FALSE;

	static LPMEASUREITEMSTRUCT lpMeasureItem;
	static LPDRAWITEMSTRUCT lpDrawItem;
	
	// Code
	switch (iMsg)
	{
		//* Message Handlers
		case WM_CREATE:

			ZeroMemory(&ps, sizeof(PAINTSTRUCT));

			//! Load Cursors
			bCursorsLoaded = LoadAppCursors(&hPickerCursor, &hDefaultCursor);
			if (!bCursorsLoaded)
			{
				DestroyWindow(hwnd);
				hwnd = NULL;
			}

			//! Menu Settings
			hMenu = LoadMenu(ghInstance, MAKEINTRESOURCE(IE_MENU));
			SetMenu(hwnd, hMenu);
			EnableMenuItem(hMenu, IDM_EDIT, MF_BYCOMMAND | MF_DISABLED);
			DrawMenuBar(hwnd);

			//! Register Server Libraries
			if (!RegisterServerLibararies())
			{
				MessageBox(NULL, TEXT("Failed To Install Required Library Files ... Exiting Now !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
				exit(EXIT_FAILURE);
			}

			//! App Log File
			if (!CreateOpenLogFile(&gpFile_AppLog, "PhotoMind.log", "a+"))
			{
				MessageBox(NULL, TEXT("Failed To Create App Log File ... Exiting Now !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
				exit(EXIT_FAILURE);
			}

			//! COM Library
			// hr = GetLibraryInterfaces(pIDesaturation, pISepia, pIColorInversion, &errorStatus);
			// if (errorStatus < 0)
			// {
			// 	MessageBox(hwnd, TEXT("ImageEditor.dll and ImageToolkit.dll Not Found ... Exiting !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
			// 	if (DEBUG == 1)
			// 		GetErrorMessage(hr, FALSE, NULL);
			// 	formattedTime = GetFormattedTime();
			// 	switch(errorStatus)
			// 	{
			// 		case -1: PrintLog(&gpFile_AppLog, "[%02d-%02d-%d %02d:%02d:%02d %s] Failed to obtain IDesaturation Interface !!!\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm); break;
			// 		case -2: PrintLog(&gpFile_AppLog, "[%02d-%02d-%d %02d:%02d:%02d %s] Failed to obtain ISepia Interface !!!\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm); break;
			// 		case -3: PrintLog(&gpFile_AppLog, "[%02d-%02d-%d %02d:%02d:%02d %s] Failed to obtain IColorInversion Interface !!!\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm); break;
			// 		default: break;
			// 	}
			// 	DestroyWindow(hwnd); 
			// }

			hr = CoCreateInstance(
				CLSID_ImageEditor,
				NULL,						
				CLSCTX_INPROC_SERVER,
				IID_Desaturation,
				(void**)&pIDesaturation
			);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain IDesaturation Interface"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr, FALSE, NULL);
				DestroyWindow(hwnd); 
				hwnd = NULL;
			}

			//! Sepia
			hr = pIDesaturation->QueryInterface(IID_ISepia, (void**)&pISepia);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain ISepia Interface"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr, FALSE, NULL);
				DestroyWindow(hwnd); 
				hwnd = NULL;
			}

			//! Color Inversion
			hr = pIDesaturation->QueryInterface(IID_IColorInversion, (void**)&pIColorInversion);
			if (FAILED(hr))
			{
				MessageBox(hwnd, TEXT("Failed to obtain IColorInversion Interface"), TEXT("COM Error"), MB_ICONERROR | MB_OK);
				GetErrorMessage(hr, FALSE, NULL);
				DestroyWindow(hwnd); 
				hwnd = NULL;
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
					SetBkColor(hdc, BLUE_BG);
					SetTextColor(hdc, RGB(44, 156, 242));
					DrawText(hdc, TEXT("Click On File Menu And Select 'Open' To Open An Image File"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					SelectObject(hdc, hOldFont);
					DeleteObject(hFont);
				}
				else
				{

					BITMAPINFO bitmapInfo;
					ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO));
					bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bitmapInfo.bmiHeader.biWidth = renderImage.cols;
					bitmapInfo.bmiHeader.biHeight = -renderImage.rows;
					bitmapInfo.bmiHeader.biPlanes = 1;
					bitmapInfo.bmiHeader.biBitCount = 24;
					bitmapInfo.bmiHeader.biCompression = BI_RGB;

					hdcMem = CreateCompatibleDC(hdc);
					

					hBitmap = CreateDIBSection(
						hdcMem,
						&bitmapInfo,
						DIB_RGB_COLORS,
						&lpBits,
						NULL,
						0
					);
					HGDIOBJ oldBmp = SelectObject(hdcMem, hBitmap);

					SetDIBits(
						hdcMem,
						hBitmap,
						0,
						renderImage.rows,
						renderImage.data,
						&bitmapInfo,
						DIB_RGB_COLORS
					);			
					
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
						renderImage.cols,
						renderImage.rows,
						SRCCOPY
					);				

					if (hdcMem)
					{
						SelectObject(hdcMem, oldBmp);
						DeleteObject(hBitmap);
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
			// if (bImageLoaded)
			// {
			// 	unsigned int pickedPixelX = GET_X_LPARAM(lParam);
			// 	unsigned int pickedPixelY = GET_Y_LPARAM(lParam);

			// 	if (pickedPixelX >= 0 && pickedPixelX <= resizedWindowWidth && pickedPixelY >= 0 && pickedPixelY <= resizedWindowHeight)
			// 		SetCursor(hPickerCursor);
			// 	else
			// 		SetCursor(hDefaultCursor);
			// }
			if (bImageLoaded)
				SetCursor(hPickerCursor);
			else
				SetCursor(hDefaultCursor);
		break;

		case WM_SETFOCUS:
			bUserRegistered = CheckUserStatus("User-Log.log");
			if (!bUserRegistered)
				DialogBox(ghInstance, MAKEINTRESOURCE(REGISTER_USER_DLG), hwnd, RegisterDialogProc);		
		break;

		case WM_LBUTTONDOWN:
			if (bImageLoaded)
			{
				giPixelX = GET_X_LPARAM(lParam);
				giPixelY = GET_Y_LPARAM(lParam);
				bColorPick = TRUE;
				InvalidateRect(hwndControlsDialog, NULL, FALSE);
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

						if (LoadOCVImage(std::string(szImagePath), &ocvImage) == false)
							MessageBox(NULL, TEXT("Failed To Load Image ... Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
						else
						{
							if (ConvertOCVImageToBGR(&ocvImage) == false)
								MessageBox(NULL, TEXT("Failed To Convert Image ... Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
							else
							{
								// if (UseGPU(std::string(szImagePath), &ocvImage))
								// 	MessageBox(NULL, TEXT("Selected GPU For Processing"), TEXT("Device Selection"), MB_OK);
								// else
								// 	MessageBox(NULL, TEXT("Selected CPU For Processing"), TEXT("Device Selection"), MB_OK);
								
								renderImage = ocvImage.clone();
								bImageLoaded = TRUE;
								EnableMenuItem(hMenu, IDM_EDIT, MF_BYCOMMAND | MF_ENABLED);
								DrawMenuBar(hwnd);
								InvalidateRect(hwnd, NULL, TRUE);
							}							
						}					
					}

				break;

				case IDM_EXIT:
					DestroyWindow(hwnd);
					hwnd = NULL;
				break;

				case IDM_EDIT:
					hwndControlsDialog = CreateDialog(ghInstance, MAKEINTRESOURCE(IE_DLG), hwnd, ControlsDialogProc);
				break;

				case IDM_GENERATE:
					DialogBox(ghInstance, MAKEINTRESOURCE(GENERATE_IMG_DLG), hwnd, GenerateImageDialogProc);		
				break;

				case IDM_ABOUT:
					DialogBox(ghInstance, MAKEINTRESOURCE(ABOUT_DLG), hwnd, AboutDialogProc);
				break;
			}
		break;

		case WM_DESTROY:

			// SafeInterfaceRelease(pIDesaturation, pISepia, pIColorInversion);
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

			DeleteAppCursor(&hPickerCursor);
			DeleteImageObject(&hBitmap);

			CloseLogFile(&gpFile_UserLog);
			CloseLogFile(&gpFile_AppLog);

			renderImage.release();
			ocvImage.release();

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
	HDC hdc = NULL, hdcPaint = NULL, hdcParent = NULL;
	HWND hwndParent = NULL;
	HBRUSH hBrush = NULL;
	PAINTSTRUCT ps;

	static char rgbBuffer[5];
	static BOOL bExportColorPickerLog = FALSE, bExportNormalizedColorPickerLog = FALSE, bCopyToClipboard = FALSE;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
			ZeroMemory(&ps, sizeof(PAINTSTRUCT));
		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(BLUE_BG);

		case WM_CTLCOLORSTATIC:
			hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(BLUE_BG);

		case WM_PAINT:
			hdcPaint = BeginPaint(hDlg, &ps);
			{
				if (bColorPick)
				{
					hwndParent = GetParent(hDlg);
					if (hwndParent == NULL)
					{
						PrintLog(&gpFile_AppLog, "Failed To Get Parent Handle in %s !!!\n", __func__);
						return (INT_PTR)FALSE;
					}

					hdcParent = GetDC(hwndParent);
					if (hdcParent == NULL)
					{
						PrintLog(&gpFile_AppLog, "Failed To Get Parent DC in %s !!!\n", __func__);
						return (INT_PTR)FALSE;
					}

					COLORREF pickedPixel = GetPixel(hdcParent, giPixelX, giPixelY);
					
					rgb.R = GetRValue(pickedPixel);
					rgb.G = GetGValue(pickedPixel);
					rgb.B = GetBValue(pickedPixel);

					snprintf(rgbBuffer, sizeof(rgbBuffer), "%d", rgb.R);
					SetDlgItemText(hDlg, ID_IMG_R, rgbBuffer);
					snprintf(rgbBuffer, sizeof(rgbBuffer), "%d", rgb.G);
					SetDlgItemText(hDlg, ID_IMG_G, rgbBuffer);
					snprintf(rgbBuffer, sizeof(rgbBuffer), "%d", rgb.B);
					SetDlgItemText(hDlg, ID_IMG_B, rgbBuffer);

					RECT colorRect = { 780, 90, 840, 150 };
					HBRUSH hBrush = CreateSolidBrush(RGB(rgb.R, rgb.G, rgb.B));
					FillRect(hdcPaint, &colorRect, hBrush);
					DeleteObject(hBrush);

					if (bExportColorPickerLog)
					{
						formattedTime = GetFormattedTime();
						PrintLog(
							&gpFile_ColorPickerLog, 
							"[%02d-%02d-%d %02d:%02d:%02d %s] Picked Color (RGB) : %d, %d, %d\n", 
							formattedTime.day, 
							formattedTime.month, 
							formattedTime.year, 
							formattedTime.hour, 
							formattedTime.minute, 
							formattedTime.second, 
							formattedTime.amPm,
							rgb.R,
							rgb.G,
							rgb.B
						);
					}

					if (bExportNormalizedColorPickerLog)
					{
						formattedTime = GetFormattedTime();
						PrintLog(
							&gpFile_NormalizedColorPickerLog, 
							"[%02d-%02d-%d %02d:%02d:%02d %s] Normalized Picked Color (RGB) : %f, %f, %f\n", 
							formattedTime.day, 
							formattedTime.month, 
							formattedTime.year, 
							formattedTime.hour, 
							formattedTime.minute, 
							formattedTime.second, 
							formattedTime.amPm,
							rgb.R / 255.0f,
							rgb.G / 255.0f,
							rgb.B / 255.0f
						);
					}

					bColorPick = FALSE;
					ReleaseDC(hwndParent, hdcParent);
					hdcParent = NULL;
					hwndParent = NULL;
				}

				if (bCopyToClipboard)
				{
					CopyToClipboard(hDlg, rgb);
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
				case ID_APPLY:

					if (bImageLoaded)
					{
						renderImage = ocvImage.clone();

						if (IsDlgButtonChecked(hDlg, ID_RB_DESAT))
						{
							//! Desaturation
							for (int yRow = 0; yRow < renderImage.rows; yRow++)
							{
								cv::Vec3b* row = renderImage.ptr<cv::Vec3b>(yRow);
								for (int xColumn = 0; xColumn < renderImage.cols; xColumn++)
								{
									COLORREF originalPixelColor = RGB(row[xColumn][2], row[xColumn][1], row[xColumn][0]);
									pIDesaturation->Desaturation(originalPixelColor, &desaturatedPixelColor);
									BYTE r = GetRValue(desaturatedPixelColor);
									BYTE g = GetGValue(desaturatedPixelColor);
									BYTE b = GetBValue(desaturatedPixelColor);
									row[xColumn] = cv::Vec3b(b, g, r);
								}
							}
							InvalidateRect(GetParent(hDlg), NULL, TRUE);
						}
						else if (IsDlgButtonChecked(hDlg, ID_RB_SEPIA))
						{
							//! Sepia
							for (int yRow = 0; yRow < renderImage.rows; yRow++)
							{
								cv::Vec3b* row = renderImage.ptr<cv::Vec3b>(yRow);
								for (int xColumn = 0; xColumn < renderImage.cols; xColumn++)
								{
									RGBColor input = 
									{
										row[xColumn][2],
										row[xColumn][1],
										row[xColumn][0],
									};

									RGBColor output;
									applySepia(input, &output);
									
									row[xColumn] = cv::Vec3b(output.b, output.g, output.r);
								}
							}
							InvalidateRect(GetParent(hDlg), NULL, TRUE);
						}
						else if (IsDlgButtonChecked(hDlg, ID_RB_INV))
						{
							//! Inversion
							for (int yRow = 0; yRow < renderImage.rows; yRow++)
							{
								cv::Vec3b* row = renderImage.ptr<cv::Vec3b>(yRow);
								for (int xColumn = 0; xColumn < renderImage.cols; xColumn++)
								{
									COLORREF originalPixelColor = RGB(row[xColumn][2], row[xColumn][1], row[xColumn][0]);
									pIColorInversion->ColorInversion(originalPixelColor, &negativePixelColor);
									BYTE r = GetRValue(negativePixelColor);
									BYTE g = GetGValue(negativePixelColor);
									BYTE b = GetBValue(negativePixelColor);
									row[xColumn] = cv::Vec3b(b, g, r);
								}
							}
							InvalidateRect(GetParent(hDlg), NULL, TRUE);
						}
						else if (IsDlgButtonChecked(hDlg, ID_RB_PIX))
						{
							applyPixelate(&renderImage, 20);
							InvalidateRect(GetParent(hDlg), NULL, TRUE);
						}
						else if (IsDlgButtonChecked(hDlg, ID_RB_GAUSBLUR))
						{
							applyGaussianBlur(&renderImage, 21);
							InvalidateRect(GetParent(hDlg), NULL, TRUE);
						}
					}
				break;

				case ID_RESET:
					renderImage = ocvImage.clone();
					CheckDlgButton(hDlg, ID_RB_DESAT, BST_UNCHECKED);
					CheckDlgButton(hDlg, ID_RB_SEPIA, BST_UNCHECKED);
					CheckDlgButton(hDlg, ID_RB_INV, BST_UNCHECKED);
					InvalidateRect(GetParent(hDlg), NULL, TRUE);
				break;

				case ID_CHK_PICK_LOG:
					if (IsDlgButtonChecked(hDlg, ID_CHK_PICK_LOG))
					{
						bExportColorPickerLog = TRUE;
						if (!CreateOpenLogFile(&gpFile_ColorPickerLog, "Color-Picker-Log.log", "a+"))
						{
							MessageBox(NULL, TEXT("Failed To Create Color Picker Log File ... Exiting Now !!!"), TEXT("PhotoMind Error"), MB_ICONERROR | MB_OK);
							exit(EXIT_FAILURE);
						}	
					}
				break;

				case ID_CHK_PICK_LOG_NL:
					if (IsDlgButtonChecked(hDlg, ID_CHK_PICK_LOG_NL))
					{
						bExportNormalizedColorPickerLog = TRUE;
						if (!CreateOpenLogFile(&gpFile_NormalizedColorPickerLog, "Normalized-Color-Picker-Log.log", "a+"))
						{
							MessageBox(NULL, TEXT("Failed To Create Normalized Color Picker Log File ... Exiting Now !!!"), TEXT("PhotoMind Error"), MB_ICONERROR | MB_OK);
							exit(EXIT_FAILURE);
						}
					}
				break;

				case ID_CHK_PICK_CLP:
					if (IsDlgButtonChecked(hDlg, ID_CHK_PICK_CLP))
						bCopyToClipboard = TRUE;
				break;
				
				case ID_ABOUT:
					DialogBox(ghInstance, MAKEINTRESOURCE(ABOUT_DLG), hDlg, AboutDialogProc);
				break;

				case ID_OK:
				case ID_EXIT:
					CloseLogFile(&gpFile_NormalizedColorPickerLog);
					CloseLogFile(&gpFile_ColorPickerLog);
					if (hdc)
						hdc = NULL;
					hwndControlsDialog = NULL;
					DestroyWindow(hDlg);
				break;
			}
		return (INT_PTR)TRUE;

		case WM_CLOSE:
			CloseLogFile(&gpFile_NormalizedColorPickerLog);
			CloseLogFile(&gpFile_ColorPickerLog);
			if (hdc)
				hdc = NULL;
			hwndControlsDialog = NULL;
			DestroyWindow(hDlg);
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
	int controlId;
	static BOOL bInit = TRUE;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
			EnableWindow(GetDlgItem(hDlg, ID_REGISTER_BTN), FALSE);
		return (INT_PTR)TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_REGISTER_BTN:
					
				if (bValidateFirstName && bValidateMiddleName && bValidateLastName)
				{
					if (!CreateOpenLogFile(&gpFile_UserLog, "User-Log.log", "w"))
					{
						MessageBox(NULL, TEXT("Failed To Create User Log File ... Exiting Now !!!"), TEXT("PhotoMind Error"), MB_ICONERROR | MB_OK);
						exit(EXIT_FAILURE);
					}

					formattedTime = GetFormattedTime();

					PrintLog(&gpFile_UserLog, "PhotoMind v1.0 User Registration Log\n");
					PrintLog(&gpFile_UserLog, "---------------------------------------------------------------\n");
					PrintLog(&gpFile_UserLog, "First Name : %s\n", user.firstName);
					PrintLog(&gpFile_UserLog, "Middle Name : %s\n", user.middleName);
					PrintLog(&gpFile_UserLog, "Last Name : %s\n", user.lastName);
					PrintLog(&gpFile_UserLog, "Date and Time : %02d-%02d-%d %02d:%02d:%02d %s\n", formattedTime.day, formattedTime.month, formattedTime.year, formattedTime.hour, formattedTime.minute, formattedTime.second, formattedTime.amPm);
					PrintLog(&gpFile_UserLog, "---------------------------------------------------------------\n");

					bUserRegistered = TRUE;

					MessageBox(NULL, TEXT("User Registered Successfully ..."), TEXT("User Registration"), MB_ICONINFORMATION | MB_OK);
					EndDialog(hDlg, (INT_PTR)0);
				}
				else
				{
					if (!bValidateFirstName && !bValidateMiddleName && !bValidateLastName)
						MessageBox(NULL, TEXT("Please Enter Valid Details !!!"), TEXT("User Registration"), MB_ICONINFORMATION | MB_OK);
					else if (!bValidateFirstName)
						MessageBox(NULL, TEXT("Please Enter Valid First Name !!!"), TEXT("User Registration"), MB_ICONINFORMATION | MB_OK);
					else if (!bValidateMiddleName)
						MessageBox(NULL, TEXT("Please Enter Valid Middle Name !!!"), TEXT("User Registration"), MB_ICONINFORMATION | MB_OK);
					else if (!bValidateLastName)
						MessageBox(NULL, TEXT("Please Enter Valid Last Name !!!"), TEXT("User Registration"), MB_ICONINFORMATION | MB_OK);
				}

				break;
			}

			switch(HIWORD(wParam))
			{
				case EN_CHANGE:

					int id = LOWORD(wParam);

					if (id == ID_FNAME_TXT)
					{
						GetDlgItemText(hDlg, ID_FNAME_TXT, user.firstName, TEXT_LENGTH);
						bValidateFirstName = ValidateUserData(user.firstName);
					}
					else if (id == ID_MNAME_TXT)
					{
						GetDlgItemText(hDlg, ID_MNAME_TXT, user.middleName, TEXT_LENGTH);
						bValidateMiddleName = ValidateUserData(user.middleName);
					}
					else if (id == ID_LNAME_TXT)
					{
						GetDlgItemText(hDlg, ID_LNAME_TXT, user.lastName, TEXT_LENGTH);
						bValidateLastName = ValidateUserData(user.lastName);
					}

					if (bValidateFirstName && bValidateMiddleName && bValidateLastName)
					{
						bInit = FALSE;
						EnableWindow(GetDlgItem(hDlg, ID_REGISTER_BTN), TRUE);
					}
					else
						EnableWindow(GetDlgItem(hDlg, ID_REGISTER_BTN), FALSE);
						
					InvalidateRect(hDlg, NULL, TRUE);

				break;
			}

		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(BLUE_BG);

		case WM_CTLCOLORSTATIC:

			hdc = (HDC)wParam;
			SetBkMode(hdc, TRANSPARENT);

			controlId = GetDlgCtrlID((HWND)lParam);
			
			if (!bInit)
			{
				if (controlId == ID_FNAME_LBL)
					SetTextColor(hdc, bValidateFirstName ? BLACK_COLOR : RED_COLOR);
				else if (controlId == ID_MNAME_LBL)
					SetTextColor(hdc, bValidateMiddleName ? BLACK_COLOR : RED_COLOR);
				else if (controlId == ID_LNAME_LBL)
					SetTextColor(hdc, bValidateLastName ? BLACK_COLOR : RED_COLOR);
			}
			else
			{
				if (controlId == ID_FNAME_LBL)
					SetTextColor(hdc, BLACK_COLOR);
				else if (controlId == ID_MNAME_LBL)
					SetTextColor(hdc, BLACK_COLOR);
				else if (controlId == ID_LNAME_LBL)
					SetTextColor(hdc, BLACK_COLOR);
			}

		return (INT_PTR)CreateSolidBrush(BLUE_BG);

		case WM_CLOSE:
			if (hdc)
				hdc = NULL;
			EndDialog(hDlg, (INT_PTR)0);
		break;

		default:
			return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}




INT_PTR CALLBACK GenerateImageDialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//*Variable Declarations
	HDC hdc = NULL;
	char outputPath[MAX_PATH] = "", escapedPath[MAX_PATH * 2];
	char promptText[1024] = "";
	DWORD threadId;

	// Code
	switch(iMsg)
	{
		case WM_INITDIALOG:
		return (INT_PTR)TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_GEN_BTN:
					OPENFILENAME ofn = { 0 };
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hDlg;
					ofn.lpstrFilter = TEXT("PNG Image (*.png)\0*.png\0");
					ofn.lpstrFile = outputPath;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_OVERWRITEPROMPT;
					ofn.lpstrDefExt = TEXT("png");
				
					if (GetSaveFileName(&ofn))
					{
						SanitizePath(outputPath, escapedPath, sizeof(escapedPath));

						GetDlgItemText(hDlg, ID_PROMPT_TXT, promptText, sizeof(promptText));
				
						if (strlen(promptText) == 0 || strlen(outputPath) == 0)
						{
							MessageBox(hDlg, TEXT("Please Enter Prompt Text and Set Output Image Path !!!"), TEXT("PhotoMind Error"), MB_OK | MB_ICONERROR);
							return (INT_PTR)TRUE;
						}
						
						//* Start Progress Dialog Thread
						HANDLE hThread = CreateThread(
							NULL,
							0,
							ShowProgressDialog,
							hDlg,
							0,
							&threadId
						);
						
						const char* result = GenerateImageUsingSD(promptText, escapedPath);
						
						//* Stop Progress Dialog Thread
						if (hwndProgressDialog)
						{
							DestroyWindow(hwndProgressDialog);
							hwndProgressDialog = NULL;
						}
						PostThreadMessage(threadId, WM_QUIT, 0, 0);

						if (result)
						{
							MessageBox(hDlg, result, TEXT("PhotoMind Image Generation"), MB_OK | MB_ICONINFORMATION);
							free((void*)result);
							result = NULL;
						}
					}	
				break;
			}
		return (INT_PTR)TRUE;

		case WM_CTLCOLORDLG:
		case WM_CTLCOLORSTATIC:
			hdc = (HDC)wParam;
			SetBkMode(hdc, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(BLUE_BG);

		case WM_CLOSE:
			if (hdc)
				hdc = NULL;
			EndDialog(hDlg, (INT_PTR)0);
		break;

		default:
			return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}


DWORD WINAPI ShowProgressDialog(LPVOID lpParam)
{
	// Variable Declarations
	MSG msg;

	// Code
	hwndProgressDialog = CreateDialog(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(ID_PROGRESS_DLG), 
		(HWND)lpParam, 
		NULL
	);

	ShowWindow(hwndProgressDialog, SW_SHOW);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if(!IsDialogMessage(hwndProgressDialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

