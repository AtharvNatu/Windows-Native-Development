@echo off

cls

cd Bin

if exist *.obj del *.obj
if exist *.exe del *.exe
if exist *.res del *.res

@REM echo ----------------------------------------------------------------------------------------------------------------
@REM echo Compiling Source Code ...
@REM echo ----------------------------------------------------------------------------------------------------------------
@REM     cl.exe /c /EHsc /std:c++20 ^
@REM     -I "..\Include" ^
@REM     -I "C:\opencv\build\include" ^
@REM     "..\Source\CPP\Client\Utils.cpp" ^
@REM     "..\Source\CPP\Client\PhotoMind.cpp"

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
    nvcc.exe -c -w -Wno-deprecated-gpu-targets --std=c++20 ^
    -I "..\Include" ^
    -I "C:\opencv\build\include" ^
    -I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8\include" ^
    "..\Source\CPP\Client\Utils.cpp" ^
    "..\Source\CPP\Client\PhotoMind.cpp" ^
    "..\Source\CPP\Client\ImageEffects.cu"

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Resource Files ...
echo ----------------------------------------------------------------------------------------------------------------
    rc.exe ..\Assets\PhotoMind.rc
move ..\Assets\PhotoMind.res ..\Bin\  > nul 2>&1

@REM @echo:
@REM echo ----------------------------------------------------------------------------------------------------------------
@REM echo Linking Libraries and Resources...
@REM echo Creating Executable...
@REM echo ----------------------------------------------------------------------------------------------------------------
@REM     link.exe *.obj *.res ^
@REM     /LIBPATH:"C:\opencv\build\x64\vc16\lib" ^
@REM     user32.lib gdi32.lib comdlg32.lib ole32.lib oleaut32.lib ^
@REM     Wbemuuid.lib Shlwapi.lib Advapi32.lib opencv_world4110.lib opencv_world4110d.lib  ^
@REM     /SUBSYSTEM:WINDOWS /OUT:PhotoMind.exe

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries and Resources...
echo Creating Executable...
echo ----------------------------------------------------------------------------------------------------------------
    link.exe *.obj *.res ^
    /LIBPATH:"C:\opencv\build\x64\vc16\lib" ^
    /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8\lib\x64" ^
    user32.lib gdi32.lib comdlg32.lib ole32.lib oleaut32.lib ^
    Wbemuuid.lib Shlwapi.lib Advapi32.lib opencv_world4110.lib opencv_world4110d.lib cudart.lib ^
    /SUBSYSTEM:WINDOWS /OUT:PhotoMind.exe

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Launching Application ...
echo ----------------------------------------------------------------------------------------------------------------
move PhotoMind.exe ..\  > nul 2>&1
cd ..
PhotoMind.exe

