#pragma once

#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <Shlwapi.h>


//! App Mode
#define DEBUG                               0

//! Icon
#define ADN_ICON                            101
#define ADN_PICKER_CURSOR                   102

//! Menu
#define IE_MENU                             201
#define IDM_OPEN                            202
#define IDM_EXIT                            203
#define IDM_EDIT                            204
#define IDM_ABOUT                           205

//! Dialogs
#define IE_DLG                              301
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
#define ID_RB_INV                           329
#define ID_APPLY                            328
#define ID_CHK_PICK_CLP                     330

#define ABOUT_DLG                           401
#define ID_ABT_OK                           402

#define REGISTER_USER_DLG                   501
#define ID_REGISTER_BTN                     502
#define ID_FNAME                            503
#define ID_MNAME                            504
#define ID_SNAME                            505
#define ID_FNAME_LBL                        506

#define WINDOW_WIDTH 	800
#define WINDOW_HEIGHT	600

#define TEXT_LENGTH     50

//* Global Callback Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ControlsDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK RegisterDialogProc(HWND, UINT, WPARAM, LPARAM);
