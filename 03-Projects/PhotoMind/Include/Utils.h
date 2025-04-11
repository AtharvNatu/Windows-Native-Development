#include "ImageToolkit.h"
#include "ImageEditor.h"

#include <iostream>
#include <cstdlib>
#include <filesystem>
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

//* COM Related
//*--------------------------------------------------------------------------------------------
BOOL RegisterServerLibararies(void);
HRESULT GetLibraryInterfaces(IDesaturation*, ISepia*, IColorInversion*, int*);
const char* GenerateImageUsingSD(const char*, const char*);
void SafeInterfaceRelease(IDesaturation*, ISepia*, IColorInversion*);
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
void CloseLogFile(FILE**);
const char* AppendPath(const char*);
FormattedTime GetFormattedTime(void);
OPENFILENAME OpenFileDialog(HWND);
void SanitizePath(const char*, char*, size_t);
void CopyToClipboard(HWND, RGB);
//*--------------------------------------------------------------------------------------------

//* Image Related
//*--------------------------------------------------------------------------------------------
bool LoadOCVImage(std::string, cv::Mat*);
bool SaveOCVImage(std::string, cv::Mat*);
bool UseGPU(std::string, cv::Mat*);
bool ConvertOCVImageToBGR(cv::Mat*);


void applySepia(RGBColor input, RGBColor *output);
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
