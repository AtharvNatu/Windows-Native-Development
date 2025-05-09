#include "ImageToolkit.h"
#include "ImageEditor.h"


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
void CreateAppFont(HFONT*, TCHAR*, int);
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
