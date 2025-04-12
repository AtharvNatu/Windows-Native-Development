@echo off

cls

cd Bin

if exist *.obj del *.obj
if exist *.exe del *.exe
if exist *.res del *.res

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
    cl.exe /c /EHsc /std:c++20 ^
    -I "..\Include" ^
    -I "C:\opencv\build\include" ^
    "..\Source\CPP\Client\Utils.cpp" ^
    "..\Source\CPP\Client\PhotoMind.cpp"

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Resource Files ...
echo ----------------------------------------------------------------------------------------------------------------
    rc.exe ..\Assets\PhotoMind.rc
move ..\Assets\PhotoMind.res ..\Bin\  > nul 2>&1

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries and Resources...
echo Creating Executable...
echo ----------------------------------------------------------------------------------------------------------------
    link.exe *.obj *.res ^
    /LIBPATH:"C:\opencv\build\x64\vc16\lib" ^
    user32.lib gdi32.lib comdlg32.lib ole32.lib oleaut32.lib Shlwapi.lib Advapi32.lib opencv_world4110.lib opencv_world4110d.lib ^
    /SUBSYSTEM:WINDOWS /OUT:PhotoMind.exe

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Launching Application ...
echo ----------------------------------------------------------------------------------------------------------------
move PhotoMind.exe ..\  > nul 2>&1
cd ..
PhotoMind.exe

