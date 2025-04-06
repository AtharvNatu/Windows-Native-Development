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
void SafeInterfaceRelease(IDesaturation*, ISepia*, IColorInversion*);
//*--------------------------------------------------------------------------------------------

//* UI Related
//*--------------------------------------------------------------------------------------------
BOOL LoadAppCursors(HCURSOR*, HCURSOR*);
void DeleteAppCursor(HCURSOR*);
void CreateAppFont(HFONT*, const TCHAR*, int);
BOOL LoadImageFromExplorer(HBITMAP*, HBITMAP*, TCHAR*);
void DeleteImageObject(HBITMAP*);
//*--------------------------------------------------------------------------------------------

//* File I/O
//*--------------------------------------------------------------------------------------------
BOOL CreateOpenLogFile(FILE**, const char*, const char*);
void PrintLog(FILE**, const char *, ...);
void CloseLogFile(FILE**);
FormattedTime GetFormattedTime(void);
OPENFILENAME OpenFileDialog(HWND);
void CopyToClipboard(HWND, RGB);
//*--------------------------------------------------------------------------------------------

//* Image Related
//*--------------------------------------------------------------------------------------------
bool LoadOCVImage(std::string imagePath, cv::Mat* image);
bool SaveOCVImage(std::string imagePath, cv::Mat* image);
bool ConvertOCVImageToBGR(cv::Mat* image);
std::vector<BYTE> GetRawPixelData(cv::Mat* image, int *error);
//*--------------------------------------------------------------------------------------------

//* User Data Related
//*--------------------------------------------------------------------------------------------
BOOL CheckUserStatus(const char*);
BOOL ValidateUserData(const char*);
//*--------------------------------------------------------------------------------------------

//* Error Handling
//*--------------------------------------------------------------------------------------------
void GetErrorMessage(HRESULT, BOOL, LPCTSTR);
void DebugMsg(TCHAR*);
//*--------------------------------------------------------------------------------------------
