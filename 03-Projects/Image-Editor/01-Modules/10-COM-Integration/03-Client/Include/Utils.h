#include "ImageToolkit.h"
#include "ImageEditor.h"

//! COM Related
void RegisterLibraries();
HRESULT GetLibraryInterfaces(IDesaturation*, ISepia*, IColorInversion*, int*);
void SafeInterfaceRelease(IDesaturation*, ISepia*, IColorInversion*);

//! UI Related
BOOL LoadAppCursors(HCURSOR*, HCURSOR*);
void DeleteAppCursor(HCURSOR*);

void CreateAppFont(HFONT*, TCHAR*, int);

BOOL LoadImageFromExplorer(HBITMAP*, HBITMAP*, TCHAR*);
void DeleteImageObject(HBITMAP*);

//! File I/O
BOOL CreateOpenLogFile(FILE*, const char*, const char*);
void PrintLog(FILE*, const char *fmt, ...);
void CloseLogFile(FILE*);

OPENFILENAME OpenFileDialog(HWND);

//! Error Handling
void GetErrorMessage(HRESULT);
void ErrorExit(LPCTSTR);
void DebugMsg(TCHAR*);
