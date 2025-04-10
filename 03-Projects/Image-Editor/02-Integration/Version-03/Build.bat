@echo off

cls

cd Bin

if exist *.obj del *.obj
if exist *.exe del *.exe
if exist *.res del *.res

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
    nvcc.exe -c -w -Wno-deprecated-gpu-targets --std=c++20 ^
    -I "C:\opencv\build\include" ^
    -I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8\include" ^
    "..\Source\CPP\Utils.cpp" ^
    "..\Source\CPP\ImageEditor.cu" ^
    "..\Source\CPP\ImageEffects.cu"

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Resource Files ...
echo ----------------------------------------------------------------------------------------------------------------
    rc.exe ..\Assets\ImageEditor.rc
move ..\Assets\ImageEditor.res ..\Bin\  > nul 2>&1

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries and Resources...
echo Creating Executable...
echo ----------------------------------------------------------------------------------------------------------------
    link.exe *.obj *.res ^
    /LIBPATH:"C:\opencv\build\x64\vc16\lib" ^
    /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8\lib\x64" ^
    user32.lib gdi32.lib comdlg32.lib ole32.lib Shlwapi.lib Advapi32.lib opencv_world4110.lib opencv_world4110d.lib cudart.lib ^
    /SUBSYSTEM:WINDOWS /OUT:ImageEditor.exe

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Launching Application ...
echo ----------------------------------------------------------------------------------------------------------------
move ImageEditor.exe ..\  > nul 2>&1
cd ..
ImageEditor.exe

