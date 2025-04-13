#pragma once

#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <string.h>


//! App Mode
#define DEBUG                               0

//! Icon
#define PM_APP_ICON                         101

//! Bitmaps
#define PM_SI_INTEL_CPU                     102
#define PM_SI_INTEL_ARC                     103
#define PM_SI_INTEL_UHD                     104
#define PM_SI_INTEL_IRIS                    105
#define PM_SI_NVIDIA                        106
#define PM_SI_AMD_CPU                       107
#define PM_SI_AMD_GPU                       108
#define PM_SI_WINDOWS                       109

//! Menu
#define PM_MENU                             201
#define IDM_OPEN                            202
#define IDM_EXIT                            203
#define IDM_EDIT                            204
#define IDM_ABOUT                           205
#define IDM_GENERATE                        206
#define IDM_SYSTEM                          207

//! Dialogs
#define PM_DLG                              301
#define ID_OK                               302
#define ID_ABOUT                            303
#define ID_EXIT                             304
#define ID_GRPBOX_EDITOR                    305
#define ID_GRPBOX_PICKER                    306
#define ID_REGISTER                         307
#define ID_GRPBOX_EFFECTS                   308
#define ID_RESET                            313
#define ID_IMG_R                            320
#define ID_IMG_G                            321
#define ID_IMG_B                            322
#define ID_CHK_PICK_LOG                     323
#define ID_CHK_PICK_LOG_NL                  324
#define ID_RB_DESAT                         326
#define ID_RB_SEPIA                         327
#define ID_APPLY                            328
#define ID_RB_INV                           329
#define ID_CHK_PICK_CLP                     330
#define ID_RB_PIX                           331
#define ID_RB_GAUSBLUR                      332

#define ABOUT_DLG                           401
#define ID_ABT_OK                           402

#define REGISTER_USER_DLG                   501
#define ID_REGISTER_BTN                     502
#define ID_FNAME_TXT                        503
#define ID_MNAME_TXT                        504
#define ID_LNAME_TXT                        505
#define ID_FNAME_LBL                        506
#define ID_MNAME_LBL                        507
#define ID_LNAME_LBL                        508

#define GENERATE_IMG_DLG                    601
#define ID_PROMPT_TXT                       602
#define ID_GEN_BTN                          603

#define ID_PROGRESS_DLG                     701
#define ID_SPIN_1                           702
#define ID_SPIN_2                           703
#define ID_SPIN_3                           704
#define ID_SPIN_4                           705

#define WINDOW_WIDTH 	                    1024
#define WINDOW_HEIGHT	                    768

#define TEXT_LENGTH                         50

//* App Colors
#define RED_COLOR                           RGB(255, 0, 0)
#define BLACK_COLOR                         RGB(0, 0, 0)
#define BLUE_FG                             RGB(197, 211, 224)
#define BLUE_BG                             RGB(197, 211, 224)

//* Global Callback Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ControlsDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK RegisterDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GenerateImageDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ProgrssDialogProc(HWND, UINT, WPARAM, LPARAM);

//* Thread Function Declarations
DWORD WINAPI ShowProgressDialog(LPVOID);

//* Function Declarations
BOOL RegisterUser(void);
void DisplaySystemDetails(HDC, int);
void DisplayStatusBar(HDC, int, int, const char*);
