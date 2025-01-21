#include "../Include/Utils.h"

//! COM Related
HRESULT GetLibraryInterfaces(IDesaturation *pIDesaturation, ISepia *pISepia, IColorInversion *pIColorInversion, int *iErrorCode)
{
    // Code
    HRESULT hr = CoCreateInstance(
        CLSID_ImageEditor,
        NULL,						
        CLSCTX_INPROC_SERVER,
        IID_Desaturation,
        (void**)&pIDesaturation
    );
    if (FAILED(hr))
    {
        *iErrorCode = -1;
        return hr;
    }

    //! Sepia
    hr = pIDesaturation->QueryInterface(IID_ISepia, (void**)&pISepia);
    if (FAILED(hr))
    {
        *iErrorCode = -2;
        return hr;
    }

    //! Color Inversion
    hr = pIDesaturation->QueryInterface(IID_IColorInversion, (void**)&pIColorInversion);
    if (FAILED(hr))
    {
        *iErrorCode = -3;
        return hr;
    }

    return hr;
}

void SafeInterfaceRelease(IDesaturation *pIDesaturation, ISepia *pISepia, IColorInversion *pIColorInversion)
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

//! UI Related
BOOL LoadAppCursors(HCURSOR *hPickerCursor, HCURSOR *hDefaultCursor)
{
    // Variable Declarations
    TCHAR cursorPath[] = TEXT("Assets/Resources/ColorPicker.cur");

    // Code
    *hPickerCursor = LoadCursorFromFile(cursorPath);
    if (hPickerCursor == NULL)
    {
        MessageBox(NULL, TEXT("Failed To Load Picker Cursor .. Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        if (DEBUG == 1)
            ErrorExit(TEXT("LoadCursorFromFile"));
        return FALSE;
    }

    *hDefaultCursor = LoadCursor(NULL, IDC_ARROW);

    return TRUE;
}

void DeleteAppCursor(HCURSOR *hCursor)
{
    // Code
    if (*hCursor)
    {
        DestroyCursor(*hCursor);
        *hCursor = NULL;
    }
}

void CreateAppFont(HFONT *hFont, TCHAR *szFontName, int iFontSize)
{
    // Code
    *hFont = CreateFont(
        iFontSize,
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
        szFontName
    );
}

BOOL LoadImageFromExplorer(HBITMAP *hBitmap, HBITMAP *hOriginalBitmap, TCHAR *szImagePath)
{
    // Code
    *hBitmap = (HBITMAP)LoadImage(NULL, szImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hBitmap == NULL)
        return FALSE;

    *hOriginalBitmap = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
    if (hOriginalBitmap == NULL)
        return FALSE;

    return TRUE;
}

void DeleteImageObject(HBITMAP *hBitmap)
{
    // Code
    if (*hBitmap)
    {
        DeleteObject(*hBitmap);
        *hBitmap = NULL;
    }
}


//! File I/O
BOOL CreateOpenLogFile(FILE *pFile, const char *szFileName, const char *szMode)
{
    // Code
    pFile = fopen(szFileName, szMode);
    if (pFile == NULL)
        return FALSE;

    return TRUE;
}

void PrintLog(FILE *pFile, const char *fmt, ...)
{
    // Variable Declarations
    va_list arg;
    int ret;

    // Code
    if (pFile == NULL)
        return;

    va_start(arg, fmt);
    {
        ret = vfprintf(pFile, fmt, arg);
    }
    va_end(arg);
}

void CloseLogFile(FILE *pFile)
{
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
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

//! Error Handling
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
		MessageBox(NULL, (LPCTSTR)buffer, TEXT("COM Debug Error"), MB_ICONERROR | MB_OK);
		LocalFree(buffer);
	}
	else
		MessageBox(NULL, TEXT("Unknown Error Code !!!"), TEXT("Unknown Error"), MB_ICONERROR | MB_OK);
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

void DebugMsg(TCHAR* szMsg)
{
    MessageBox(NULL, szMsg, TEXT("Status"), MB_OK);
}

