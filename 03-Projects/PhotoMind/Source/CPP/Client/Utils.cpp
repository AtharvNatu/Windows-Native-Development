#include "Utils.h"

//! COM Related
//*--------------------------------------------------------------------------------------------
BOOL RegisterDLL(const TCHAR *szServerName)
{
    // Variable Declarations
    HMODULE hDll = NULL;
    typedef HRESULT(__stdcall *DLLRegisterServerProc)(void);

    // Code
    hDll = LoadLibrary(szServerName);
    if (hDll == NULL)
        return FALSE;

    DLLRegisterServerProc pDLLRegisterServer = (DLLRegisterServerProc)GetProcAddress(hDll, "DllRegisterServer");
    if (pDLLRegisterServer == NULL)
    {
        FreeLibrary(hDll);
        hDll = NULL;
        return FALSE;
    }

    HRESULT hr = pDLLRegisterServer();
    if (FAILED(hr))
    {
        FreeLibrary(hDll);
        hDll = NULL;
        return FALSE;
    }

    FreeLibrary(hDll);
    hDll = NULL;
    return TRUE;
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

const char* GenerateImageUsingSD(const char* promptText, const char* outputPath)
{
    // Variable Declarations
    CLSID clsid_SdServer;
    IDispatch* pIDispatch = NULL;
    DISPID dispId_GenerateImage;
    LPCOLESTR szFunctionName = L"GenerateImage";
    VARIANT vArg[2];
    VARIANT vRetval;
    DISPPARAMS param;
    BSTR olePrompt = NULL, oleOutputPath = NULL;
    HRESULT hr = S_OK;
    static char buffer[1024];

    // Code

    // Convert ANSI to BSTR
    int promptWideCharLength = MultiByteToWideChar(CP_ACP, 0, promptText, -1, NULL, 0);
    olePrompt = SysAllocStringLen(0, promptWideCharLength - 1);
    MultiByteToWideChar(CP_ACP, 0, promptText, -1, olePrompt, promptWideCharLength - 1);

    int outputPathWideCharLength = MultiByteToWideChar(CP_ACP, 0, outputPath, -1, NULL, 0);
    oleOutputPath = SysAllocStringLen(NULL, outputPathWideCharLength - 1);
    MultiByteToWideChar(CP_ACP, 0, outputPath, -1, oleOutputPath, outputPathWideCharLength - 1);

    hr = CLSIDFromProgID(L"StableDiffusion.Server", &clsid_SdServer);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("CLSIDFromProgID Failed For StableDiffusion.Server !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
        return NULL;
    }
   
    hr = CoCreateInstance(
        clsid_SdServer,
        NULL,
        CLSCTX_LOCAL_SERVER,
        IID_IDispatch,
        (void**)&pIDispatch
    );
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed To Obtain Implemented IDispatch Interface of StableDiffusion.Server !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
        return NULL;
    }

    VariantInit(&vArg[0]);
    VariantInit(&vArg[1]);
    {
        vArg[0].vt = VT_BSTR;
        vArg[0].bstrVal = oleOutputPath;
        
        vArg[1].vt = VT_BSTR;
        vArg[1].bstrVal = olePrompt;

        param.rgvarg = vArg;
        param.cArgs = 2;
        param.cNamedArgs = 0;
        param.rgdispidNamedArgs = NULL;

        VariantInit(&vRetval);
        {
            // Get DISPID of GenerateImage()
            hr = pIDispatch->GetIDsOfNames(
                IID_NULL,
                (LPOLESTR*)&szFunctionName,
                1,
                GetUserDefaultLCID(),
                &dispId_GenerateImage
            );
            if (FAILED(hr))
            {
                MessageBox(NULL, TEXT("Failed To Obtain ID For GenerateImage() !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
                return NULL;
            }

            hr = pIDispatch->Invoke(
                dispId_GenerateImage,
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
                MessageBox(NULL, TEXT("Failed To Invoke GenerateImage() !!!"), TEXT("PhotoMind"), MB_ICONERROR | MB_OK);
                return NULL;
            }
            else
            {
                if (vRetval.vt == VT_BSTR)
                {
                    int length = WideCharToMultiByte(CP_ACP, 0, vRetval.bstrVal, -1, NULL, 0, NULL, NULL);
                    if (length > 0)
                        WideCharToMultiByte(CP_ACP, 0, vRetval.bstrVal, -1, buffer, sizeof(buffer), NULL, NULL);
                    else
                        snprintf(buffer, sizeof(buffer), "Error Occurred While Generating Image !!!");
                }
                else
                    snprintf(buffer, sizeof(buffer), "Error Occurred While Generating Image !!!");    
            }
        }
        VariantClear(&vRetval);

        if (oleOutputPath)
        {
            SysFreeString(oleOutputPath);
            oleOutputPath = NULL;
        }

        if (olePrompt)
        {
            SysFreeString(olePrompt);
            olePrompt = NULL;
        }

    }
    VariantClear(&vArg[1]);
    VariantClear(&vArg[0]);

    if (pIDispatch)
	{
		pIDispatch->Release();
		pIDispatch = NULL;
	}

    return buffer;
}

BOOL GetSystemDetails(SYSINFO *sysInfo)
{
    // Variable Declarations
    IWbemLocator* pIWbemLocator = NULL;
    IWbemServices* pIWbemServices = NULL;
    IWbemClassObject* pIWbemClassObject = NULL;
    IEnumWbemClassObject *pIEnumWbemClassObject = NULL;
    VARIANT vtProp;
    ULONG uRet = 0;
    HRESULT hr = S_OK;

    // Code
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
        return FALSE;

    hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL
    );
    if (FAILED(hr))
    {
        CoUninitialize();
        return FALSE;
    }

    hr = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pIWbemLocator
    );
    if (FAILED(hr))
    {
        CoUninitialize();
        return FALSE;
    }

    hr = pIWbemLocator->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pIWbemServices
    );
    if (FAILED(hr))
    {
        pIWbemLocator->Release();
        pIWbemLocator = NULL;
        CoUninitialize();
        return FALSE;
    }

    hr = CoSetProxyBlanket(
        pIWbemServices,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE
    );
    if (FAILED(hr))
    {
        pIWbemServices->Release();
        pIWbemServices = NULL;
        pIWbemLocator->Release();
        pIWbemLocator = NULL;
        CoUninitialize();
        return FALSE;
    }

    //* Query OS Info
    hr = pIWbemServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT Caption, Version FROM Win32_OperatingSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pIEnumWbemClassObject
    );
    if (FAILED(hr))
    {
        pIWbemServices->Release();
        pIWbemServices = NULL;
        pIWbemLocator->Release();
        pIWbemLocator = NULL;
        CoUninitialize();
        return FALSE;
    }

    if (pIEnumWbemClassObject->Next(
        WBEM_INFINITE,
        1,
        &pIWbemClassObject,
        &uRet) == S_OK)
    {
        VariantInit(&vtProp);
        {
            pIWbemClassObject->Get(L"Caption", 0, &vtProp, NULL, NULL);
            sysInfo->osName = vtProp.bstrVal;
            VariantClear(&vtProp);

            pIWbemClassObject->Get(L"Version", 0, &vtProp, NULL, NULL);
            sysInfo->osVersion = vtProp.bstrVal;
            VariantClear(&vtProp);
        }
        VariantClear(&vtProp);

        pIWbemClassObject->Release();
        pIWbemClassObject = NULL;
    }

    pIEnumWbemClassObject->Release();
    pIEnumWbemClassObject = NULL;

    //* Query CPU
    hr = pIWbemServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT Name, NumberOfCores, NumberOfLogicalProcessors FROM Win32_Processor"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pIEnumWbemClassObject
    );
    if (FAILED(hr))
    {
        pIWbemServices->Release();
        pIWbemServices = NULL;
        pIWbemLocator->Release();
        pIWbemLocator = NULL;
        CoUninitialize();
        return FALSE;
    }

    if (pIEnumWbemClassObject->Next(
        WBEM_INFINITE,
        1,
        &pIWbemClassObject,
        &uRet) == S_OK)
    {
        VariantInit(&vtProp);
        {
            pIWbemClassObject->Get(L"Name", 0, &vtProp, NULL, NULL);
            sysInfo->cpuName = vtProp.bstrVal;
            VariantClear(&vtProp);

            pIWbemClassObject->Get(L"NumberOfCores", 0, &vtProp, NULL, NULL);
            sysInfo->cpuCores = vtProp.intVal;
            VariantClear(&vtProp);

            pIWbemClassObject->Get(L"NumberOfLogicalProcessors", 0, &vtProp, NULL, NULL);
            sysInfo->cpuThreads = vtProp.intVal;
        }
        VariantClear(&vtProp);

        pIWbemClassObject->Release();
        pIWbemClassObject = NULL;
    }

    //* Query RAM
    hr = pIWbemServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT TotalPhysicalMemory FROM Win32_ComputerSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pIEnumWbemClassObject
    );
    if (FAILED(hr))
    {
        pIWbemServices->Release();
        pIWbemServices = NULL;
        pIWbemLocator->Release();
        pIWbemLocator = NULL;
        CoUninitialize();
        return FALSE;
    }

    if (pIEnumWbemClassObject->Next(
        WBEM_INFINITE,
        1,
        &pIWbemClassObject,
        &uRet) == S_OK)
    {
        VariantInit(&vtProp);
        {
            pIWbemClassObject->Get(L"TotalPhysicalMemory", 0, &vtProp, NULL, NULL);
            sysInfo->ram = _wtoi64(vtProp.bstrVal);
        }
        VariantClear(&vtProp);

        pIWbemClassObject->Release();
        pIWbemClassObject = NULL;
    }

    pIEnumWbemClassObject->Release();
    pIEnumWbemClassObject = NULL;

    //* Query GPU
    hr = pIWbemServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT Name, AdapterRAM FROM Win32_VideoController"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pIEnumWbemClassObject
    );
    if (FAILED(hr))
    {
        pIWbemServices->Release();
        pIWbemServices = NULL;
        pIWbemLocator->Release();
        pIWbemLocator = NULL;
        CoUninitialize();
        return FALSE;
    }

    if (pIEnumWbemClassObject->Next(
        WBEM_INFINITE,
        1,
        &pIWbemClassObject,
        &uRet) == S_OK)
    {
        VariantInit(&vtProp);
        {
            pIWbemClassObject->Get(L"Name", 0, &vtProp, NULL, NULL);
            sysInfo->gpuName = vtProp.bstrVal;
            VariantClear(&vtProp);

            pIWbemClassObject->Get(L"AdapterRAM", 0, &vtProp, NULL, NULL);
            sysInfo->gpuVRAM = vtProp.ullVal;
        }
        VariantClear(&vtProp);

        pIWbemClassObject->Release();
        pIWbemClassObject = NULL;
    }

    pIEnumWbemClassObject->Release();
    pIEnumWbemClassObject = NULL;

    pIWbemServices->Release();
    pIWbemServices = NULL;
    pIWbemLocator->Release();
    pIWbemLocator = NULL;

    CoUninitialize();

    return TRUE;
}

CPU GetCPUInfo(std::wstring cpuName)
{
    // Code
    std::wstring lowerCaseName = cpuName;
    std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(), ::tolower);

    if (lowerCaseName.find(L"intel") != std::wstring::npos)
        return CPU::Intel;
    else
        return CPU::AMD;
}

GPU GetGPUInfo(std::wstring gpuName)
{
    // Code
    std::wstring lowerCaseName = gpuName;
    std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(), ::tolower);

    if (lowerCaseName.find(L"uhd") != std::wstring::npos)
        return GPU::UHD;
    else if (lowerCaseName.find(L"iris") != std::wstring::npos)
        return GPU::Iris;
    else if (lowerCaseName.find(L"arc") != std::wstring::npos)
        return GPU::ARC;
    else if (lowerCaseName.find(L"nvidia") != std::wstring::npos)
        return GPU::Nvidia;
    else if (lowerCaseName.find(L"amd") != std::wstring::npos)
        return GPU::Radeon;
   
    return GPU::Undefined;
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
    TCHAR cursorPath[] = TEXT("Assets/Resources/Cursors/Color-Picker-Red.cur");

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

void CreateAppFont(HFONT *hFont, const TCHAR *szFontName, int iFontSize)
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

//*--------------------------------------------------------------------------------------------

//! File I/O
//*--------------------------------------------------------------------------------------------
BOOL CreateOpenLogFile(FILE **ppFile, const char *szFileName, const char *szMode)
{
    // Code
    const char* szFilePath = AppendPath(szFileName);
    *ppFile = fopen(szFilePath, szMode);
    if (*ppFile == NULL)
        return FALSE;   
    return TRUE;
}

void PrintLog(FILE **ppFile, const char *fmt, ...)
{
    // Variable Declarations
    va_list args;

    // Code
    if (*ppFile == NULL)
        return;

    va_start(args, fmt);
    {
        vfprintf(*ppFile, fmt, args);
    }
    va_end(args);
}

void PrintLogWithTime(FILE **ppFile, const char *fmt, ...)
{
    // Variable Declarations
    char timePrefix[64];
    char logMsg[1024];
    char buffer[8192];
    va_list args;

    // Code
    if (*ppFile == NULL)
        return;
    
    FormattedTime formattedTime = GetFormattedTime();

    snprintf(
        timePrefix,
        sizeof(timePrefix),
        "[%02d-%02d-%04d %02d:%02d:%02d %s] ",
        formattedTime.day,
        formattedTime.month,
        formattedTime.year,
        formattedTime.hour,
        formattedTime.minute,
        formattedTime.second,
        formattedTime.amPm
    );

    va_start(args, fmt);
    {
        vsnprintf(logMsg, sizeof(logMsg), fmt, args);
    }
    va_end(args);

    snprintf(buffer, sizeof(buffer), "%s%s", timePrefix, logMsg);

    PrintLog(ppFile, "%s", buffer);
}

void CloseLogFile(FILE **ppFile)
{
    if (*ppFile)
    {
        fclose(*ppFile);
        *ppFile = NULL;
    }
}

const char* AppendPath(const char* fileName)
{
    // Variable Declarations
    TCHAR appPath[MAX_PATH];
    static char absoluteFilePath[MAX_PATH];

    // Code
    if (GetModuleFileName(NULL, appPath, MAX_PATH) == 0)
        return NULL;

    PathRemoveFileSpec(appPath);

    PathCombine(absoluteFilePath, appPath, fileName);

    return absoluteFilePath;
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
    formattedTime.amPm = (systemTime.wHour >= 12) ? "PM" : "AM";

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
	ofn.lpstrFilter = TEXT("Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.webp)\0*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.webp\0All Files (*.*)\0*.*\0");
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
	ofn.lpstrDefExt = TEXT("jpg");
	ofn.lCustData = 0L;
	ofn.lpTemplateName = NULL;

	return ofn;
}

const char* SaveFileDialog(HWND hwndOwner)
{
	// Code
	OPENFILENAME ofn;
	static TCHAR szFileName[_MAX_PATH];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	szFileName[0] = '\0';

    ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndOwner;
	ofn.hInstance = NULL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
    ofn.lpstrFilter = TEXT("Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.webp)\0*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.webp\0All Files (*.*)\0*.*\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = TEXT("Save Image");
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("png");
	ofn.lCustData = 0L;
	ofn.lpTemplateName = NULL;

	if (GetSaveFileName(&ofn))
        return szFileName;

    return NULL;
}

void SanitizePath(const char* input, char* output, size_t maxLength)
{
    // Code
    size_t i = 0;

    for (size_t j = 0; input[j] != '\0'; ++j)
    {
        if (i >= maxLength - 1)
            break;
        
        if (input[j] == '\\')
        {
            if (i < maxLength - 2)
            {
                output[i++] = '\\';
                output[i++] = '\\';
            }
            else
                break;
        }
        else
            output[i++] = input[j];
    }
    output[i] = '\0';
}

void CopyToClipboard(HWND hwnd, RGB rgb)
{
    // Variable Declarations
    char buffer[40];

    // Code
    snprintf(buffer, sizeof(buffer), "R : %d, G : %d, B : %d", rgb.R, rgb.G, rgb.B);
    const size_t bufferLength = strlen(buffer) + 1;

    HGLOBAL hGlobalMemory = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bufferLength);
    memcpy(GlobalLock(hGlobalMemory), buffer, bufferLength);
    GlobalUnlock(hGlobalMemory);
    OpenClipboard(hwnd);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hGlobalMemory);
    GlobalFree(hGlobalMemory);
    CloseClipboard();
}

//*--------------------------------------------------------------------------------------------

//! User Data Related
//*--------------------------------------------------------------------------------------------
BOOL CheckUserStatus(const char* fileName)
{
    // Variable Declarations
    TCHAR appPath[MAX_PATH], absoluteFilePath[MAX_PATH];

    // Code
    if (GetModuleFileName(NULL, appPath, MAX_PATH) == 0)
        return FALSE;

    PathRemoveFileSpec(appPath);

    PathCombine(absoluteFilePath, appPath, fileName);

    return PathFileExists(absoluteFilePath);
}

BOOL ValidateUserData(const char *szData)
{
	// Code
	for (int i = 0; i < strlen(szData); i++)
	{   
		if (!isalpha(szData[i]))
			return FALSE;
	}
    
	return TRUE;
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

//* Image Related
//*--------------------------------------------------------------------------------------------
BOOL LoadImageFromExplorer(HBITMAP *hBitmap, HBITMAP *hOriginalBitmap, TCHAR *szImagePath)
{
    // Code
    *hBitmap = (HBITMAP)LoadImage(NULL, szImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (*hBitmap == NULL)
        return FALSE;

    *hOriginalBitmap = (HBITMAP)CopyImage(*hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
    if (*hOriginalBitmap == NULL)
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

BOOL LoadOCVImage(std::string imagePath, cv::Mat* image)
{
    // Code
    *image = cv::imread(cv::String(imagePath), cv::IMREAD_UNCHANGED);
    if (!image->data)
        return FALSE;
    return TRUE;
}

BOOL SaveOCVImage(std::string imagePath, cv::Mat* image)
{
    // Code
    if (!cv::imwrite(cv::String(imagePath), *image))
        return FALSE;
    return TRUE;
}

BOOL UseGPU(std::string imagePath, cv::Mat* image)
{
    // Code
    constexpr size_t imageSizeThreshold = 10000000; // 10 MB
    constexpr size_t imageWidthThreshold = 3840;

    std::uintmax_t imageSize = std::filesystem::file_size(imagePath);
    int imageWidth = image->size().width;
    if (imageSize > imageSizeThreshold || imageWidth >= imageWidthThreshold)
        return TRUE;
    
    return FALSE;
}

BOOL ConvertOCVImageToBGR(cv::Mat* image)
{
    // Code
    cv::Mat temp;

    int imageChannels = image->channels();

    switch(imageChannels)
    {
        case 1:
            cv::cvtColor(*image, temp, cv::COLOR_GRAY2BGR);
        break;

        case 3:
            temp = image->clone();
        break;

        case 4:
            cv::cvtColor(*image, temp, cv::COLOR_BGRA2BGR);
        break;

        default:
            return FALSE;
    }

    *image = temp;

    return TRUE;
}


std::vector<BYTE> GetRawPixelData(cv::Mat* image)
{
    std::vector<BYTE> imageVector;

    if (image->isContinuous())
        imageVector.assign((BYTE*)image->datastart, (BYTE*)image->dataend);
    else
    {
        for (int i = 0; i < image->rows; i++)
        {
            imageVector.insert(
                imageVector.end(), 
                image->ptr<BYTE>(i), 
                image->ptr<BYTE>(i) + image->cols
            );
        }
    }

    return imageVector;
}

void applySepia(RGBColor input, RGBColor *output)
{
	float tr = 0.393f * input.r + 0.769f * input.g + 0.189f * input.b;
	float tg = 0.349f * input.r + 0.686f * input.g + 0.168f * input.b;
	float tb = 0.272f * input.r + 0.534f * input.g + 0.131f * input.b;

	output->r = (BYTE)std::min(255.0f, tr);
	output->g = (BYTE)std::min(255.0f, tg);
	output->b = (BYTE)std::min(255.0f, tb);
}

void applyContrast(RGBColor input, RGBColor* output, float contrast)
{
    // Clamp contrast to a reasonable range (e.g., -255 to 255)
    contrast = std::max(-255.0f, std::min(255.0f, contrast));

    // Normalize contrast value
    float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));

    output->r = (BYTE)std::clamp(factor * (input.r - 128.0f) + 128.0f, 0.0f, 255.0f);
    output->g = (BYTE)std::clamp(factor * (input.g - 128.0f) + 128.0f, 0.0f, 255.0f);
    output->b = (BYTE)std::clamp(factor * (input.b - 128.0f) + 128.0f, 0.0f, 255.0f);
}


void applyPixelate(cv::Mat* image, int blockSize)
{
    for (int y = 0; y < image->rows; y += blockSize)
    {
        for (int x = 0; x < image->cols; x += blockSize)
        {
            cv::Vec3i avgColor(0, 0, 0);
            int count = 0;

            for (int j = 0; j < blockSize && y + j < image->rows; ++j)
            {
                for (int i = 0; i < blockSize && x + i < image->cols; ++i)
                {
                    cv::Vec3b color = image->at<cv::Vec3b>(y + j, x + i);
                    avgColor += cv::Vec3i(color[0], color[1], color[2]);
                    ++count;
                }
            }

            cv::Vec3b finalColor(
                avgColor[0] / count,
                avgColor[1] / count,
                avgColor[2] / count
            );

            for (int j = 0; j < blockSize && y + j < image->rows; ++j)
            {
                for (int i = 0; i < blockSize && x + i < image->cols; ++i)
                {
                    image->at<cv::Vec3b>(y + j, x + i) = finalColor;
                }
            }
        }
    }
}

void applyGaussianBlur(cv::Mat* image, int kernelSize)
{
    if (kernelSize % 2 == 0) kernelSize += 1; // Kernel size must be odd
    if (kernelSize <= 1) return;

    cv::Mat blurred;
    cv::GaussianBlur(*image, blurred, cv::Size(kernelSize, kernelSize), 0);
    blurred.copyTo(*image); // Overwrite original
}


//*--------------------------------------------------------------------------------------------


