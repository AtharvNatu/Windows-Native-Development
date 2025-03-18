#pragma once

#include <windows.h>

//? C Header Files
#include <stdlib.h>
#include <math.h>

//? Direct2D Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

//* Macros
template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
    #if defined( DEBUG ) || defined( _DEBUG )
        #define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
    #else
        #define Assert(b)
    #endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
    EXTERN_C IMAGE_DOS_HEADER __ImageBase;
    #define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#define ADN_ICON 101

//* Function Declarations
HRESULT CreateDeviceIndependentResources();
HRESULT CreateDeviceResources();
void DiscardDeviceResources();

