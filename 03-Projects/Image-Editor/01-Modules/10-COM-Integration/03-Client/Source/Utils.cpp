#include "../Include/Utils.h"

//! COM Related
//*--------------------------------------------------------------------------------------------
BOOL RegisterDLL(char *szServerName)
{
    // Variable Declarations
    char szShellCommand[512];

    // Code
    snprintf(szShellCommand, sizeof(szShellCommand), "regsvr32.exe \"%s\"", szServerName);

    HINSTANCE hInst = ShellExecute(
        NULL,
        "runas",
        "regsvr32",
        szShellCommand,
        NULL,
        SW_SHOW
    );

    if ((INT_PTR)hInst > 32)
        return TRUE;

    return FALSE;
}

BOOL RegisterServerLibararies(void)
{
    // Variable Declarations
    HKEY hCLSIDKeys[2];
    LONG lResult;
    BOOL bKey1_Exists = FALSE, bKey2_Exists = NULL;

    // Code
    lResult = RegOpenKeyEx(
        HKEY_CLASSES_ROOT, 
        TEXT("CLSID\\{A0E209B2-7EEE-4C54-ADD3-ADFAFA46E9A1}"),
        0,
        KEY_READ,
        &hCLSIDKeys[0]
    );

    if (lResult != ERROR_SUCCESS)
        bKey1_Exists = FALSE;
    else
        bKey1_Exists = TRUE;

    lResult = RegOpenKeyEx(
        HKEY_CLASSES_ROOT, 
        TEXT("CLSID\\{2F5B06BE-BF26-4AC1-B64F-5DE34974419D}"),
        0,
        KEY_READ,
        &hCLSIDKeys[1]
    );

    if (lResult != ERROR_SUCCESS)
        bKey2_Exists = FALSE;
    else
        bKey2_Exists = TRUE;

    if (bKey1_Exists == FALSE && bKey2_Exists == FALSE)
    {   
        if (RegisterDLL("ImageToolkit.dll") && RegisterDLL("ImageEditor.dll"))
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        RegCloseKey(hCLSIDKeys[1]);
        RegCloseKey(hCLSIDKeys[0]);
    }

    return TRUE;
}

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
//*--------------------------------------------------------------------------------------------


//! UI Related
//*--------------------------------------------------------------------------------------------
BOOL LoadAppCursors(HCURSOR *hPickerCursor, HCURSOR *hDefaultCursor)
{
    // Variable Declarations
    TCHAR cursorPath[] = TEXT("Assets/Resources/ColorPicker.cur");

    // Code
    *hPickerCursor = LoadCursorFromFile(cursorPath);
    if (*hPickerCursor == NULL)
    {
        MessageBox(NULL, TEXT("Failed To Load App Picker Cursor .. Exiting !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        if (DEBUG == 1)
            GetErrorMessage(NULL, TRUE, TEXT("LoadCursorFromFile"));
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

//*--------------------------------------------------------------------------------------------

//! File I/O
//*--------------------------------------------------------------------------------------------
BOOL CreateOpenLogFile(FILE **ppFile, const char *szFileName, const char *szMode)
{
    // Code
    *ppFile = fopen(szFileName, szMode);
    if (*ppFile == NULL)
        return FALSE;

    return TRUE;
}

void PrintLog(FILE **ppFile, const char *fmt, ...)
{
    // Variable Declarations
    va_list arg;
    int ret;

    // Code
    if (*ppFile == NULL)
        return;

    va_start(arg, fmt);
    {
        ret = vfprintf(*ppFile, fmt, arg);
    }
    va_end(arg);
}

void CloseLogFile(FILE **ppFile)
{
    if (*ppFile)
    {
        fclose(*ppFile);
        *ppFile = NULL;
    }
}

FormattedTime GetFormattedTime(void)
{
    // Variable Declarations
    FormattedTime formattedTime;
    SYSTEMTIME systemTime;

    // Code
    GetLocalTime(&systemTime);

    memset(&formattedTime, 0, sizeof(formattedTime));

    formattedTime.hour = systemTime.wHour % 12;
    if (formattedTime.hour == 0)
        formattedTime.hour = 12;

    formattedTime.minute = systemTime.wMinute;
    formattedTime.second = systemTime.wSecond;
    formattedTime.amPm = (formattedTime.hour >= 12) ? "PM" : "AM";

    formattedTime.day = systemTime.wDay;
    formattedTime.month = systemTime.wMonth;
    formattedTime.year = systemTime.wYear;

    return formattedTime;

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
//*--------------------------------------------------------------------------------------------

//! Error Handling
//*--------------------------------------------------------------------------------------------
void GetErrorMessage(HRESULT hr, BOOL bLastError, LPCTSTR lpszFunction)
{
	// Variable Declarationss
	LPVOID lpMsgBuffer;
    DWORD dw;

    if (bLastError)
    {
        LPVOID lpDisplayBuffer;

        DWORD dw = GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&lpMsgBuffer,
            0,
            NULL
        );

        lpDisplayBuffer = (LPVOID)LocalAlloc(
            LMEM_ZEROINIT,
            (lstrlen((LPCTSTR)lpMsgBuffer) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)
        );
        StringCchPrintf(
            (LPTSTR)lpDisplayBuffer,
            LocalSize(lpDisplayBuffer) / sizeof(TCHAR),
            TEXT("%s() Failed With Error %d : %s"),
            lpszFunction,
            dw,
            lpMsgBuffer
        );

        MessageBox(NULL, (LPCTSTR)lpMsgBuffer, TEXT("Win32 API Error"), MB_ICONERROR | MB_OK);

        LocalFree(lpMsgBuffer);
        LocalFree(lpDisplayBuffer);

    }
    else
    {
        dw = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&lpMsgBuffer,
            0,
            NULL
        );

        if (dw != 0)
        {
            MessageBox(NULL, (LPCTSTR)lpMsgBuffer, TEXT("COM Debug Error"), MB_ICONERROR | MB_OK);
            LocalFree(lpMsgBuffer);
        }
        else
            MessageBox(NULL, TEXT("Unknown Error Code !!!"), TEXT("Unknown Error"), MB_ICONERROR | MB_OK);
    }
	
}

void DebugMsg(TCHAR* szMsg)
{
    MessageBox(NULL, szMsg, TEXT("Status"), MB_OK);
}
//*--------------------------------------------------------------------------------------------
