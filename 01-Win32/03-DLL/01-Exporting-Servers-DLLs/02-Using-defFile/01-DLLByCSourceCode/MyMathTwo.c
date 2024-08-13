#include <windows.h>

//* DLLs Entry-Point Function
BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
    // Code
    switch(dwReason)
    {
        case DLL_PROCESS_ATTACH:
        break;

        case DLL_THREAD_ATTACH:
        break;

        case DLL_THREAD_DETACH:
        break;

        case DLL_PROCESS_DETACH:
        break;

        default:
        break;
    }

    return TRUE;
}

//* DLL's Functionality
//* ---------------------------------------------------------------------------------------------

//? DLL's Internal Function(s)
void CheckNumber(int num)
{
    if (num < 0)
        MessageBox(NULL, TEXT("Number is negative !"), TEXT("Number Check"), MB_ICONINFORMATION | MB_OK);
}

//? Export Function(s)
int MakeCube(int num)
{
    CheckNumber(num);
    return num * num * num;
}

//* ---------------------------------------------------------------------------------------------
