#include "ImageToolkit.h"
#include "ImageEditor.h"

// COM
#include <comdef.h>
#include <WbemIdl.h>

// C++ Headers
#include <iostream>
#include <cstdlib>
#include <filesystem>

// OpenCV Headers
#include <opencv2/opencv.hpp>

//* Global Variable Declarations
typedef struct tagTimeStruct
{
    int day;
    int month;
    int year;
    int hour;
    int minute;
    int second;
    const char *amPm;
} FormattedTime;

typedef struct tagUser
{
	char firstName[TEXT_LENGTH];
	char middleName[TEXT_LENGTH];
	char lastName[TEXT_LENGTH];
} USER;

typedef struct tagRGB
{
	unsigned int R;
	unsigned int G;
	unsigned int B;
} RGB;

typedef struct tagRGBColor
{
    BYTE r;
    BYTE g;
    BYTE b;
} RGBColor;

typedef struct tagSystemDetails
{
    std::wstring osName;
    std::wstring osVersion;
    std::wstring cpuName;
    int cpuCores;
    int cpuThreads;
    size_t ram;
    std::wstring gpuName;
    size_t gpuVRAM;
} SYSINFO;

enum CPU
{
    Intel,
    AMD
};

enum GPU
{
    UHD,
    Iris,
    ARC,
    Nvidia,
    Radeon,
    Undefined
};

enum HPP
{
    CUDA,
    OpenCL
};

//* COM Related
//*--------------------------------------------------------------------------------------------
BOOL RegisterServerLibararies(void);
HRESULT GetLibraryInterfaces(IDesaturation*, ISepia*, IColorInversion*, int*);
const char* GenerateImageUsingSD(const char*, const char*);
void SafeInterfaceRelease(IDesaturation*, ISepia*, IColorInversion*);
BOOL GetSystemDetails(SYSINFO*);
CPU GetCPUInfo(std::wstring);
GPU GetGPUInfo(std::wstring);
//*--------------------------------------------------------------------------------------------

//* UI Related
//*--------------------------------------------------------------------------------------------
BOOL LoadAppCursors(HCURSOR*, HCURSOR*);
void DeleteAppCursor(HCURSOR*);
void CreateAppFont(HFONT*, const TCHAR*, int);
//*--------------------------------------------------------------------------------------------

//* File I/O
//*--------------------------------------------------------------------------------------------
BOOL CreateOpenLogFile(FILE**, const char*, const char*);
void PrintLog(FILE**, const char *, ...);
void PrintLogWithTime(FILE**, const char *, ...);
void CloseLogFile(FILE**);
const char* AppendPath(const char*);
FormattedTime GetFormattedTime(void);
OPENFILENAME OpenFileDialog(HWND);
const char* SaveFileDialog(HWND hwndOwner);
void SanitizePath(const char*, char*, size_t);
void CopyToClipboard(HWND, RGB);
//*--------------------------------------------------------------------------------------------

//* Image Related
//*--------------------------------------------------------------------------------------------
BOOL LoadImageFromExplorer(HBITMAP*, HBITMAP*, TCHAR*);
void DeleteImageObject(HBITMAP*);

BOOL LoadOCVImage(std::string, cv::Mat*);
BOOL SaveOCVImage(std::string, cv::Mat*);
BOOL UseGPU(std::string, cv::Mat*);
BOOL ConvertOCVImageToBGR(cv::Mat*);


void applyDesaturation(cv::Mat&);
void applySepia(cv::Mat&);
void applyColorInversion(cv::Mat&);
void applyPixelate(cv::Mat* image, int blockSize);
void applyGaussianBlur(cv::Mat* image, int kernelSize);

//*--------------------------------------------------------------------------------------------

//* User Data Related
//*--------------------------------------------------------------------------------------------
BOOL ReadUserLogFileName(FILE**, char*, size_t);
BOOL CheckUserStatus(const char*);
BOOL ValidateUserData(const char*);
//*--------------------------------------------------------------------------------------------

//* Error Handling
//*--------------------------------------------------------------------------------------------
void GetErrorMessage(HRESULT, BOOL, LPCTSTR);
void DebugMsg(TCHAR*);
//*--------------------------------------------------------------------------------------------
