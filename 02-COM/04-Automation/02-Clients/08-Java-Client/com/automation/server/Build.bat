@echo off

set JAVA_INCLUDE_PATH="C:\Program Files\Java\jdk-24\include"

cls

if exist *.obj del *.obj
if exist *.exe del *.exe
if exist *.res del *.res

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling JNI and Win32 Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
cl.exe  /c ^
        /EHsc ^
        /I %JAVA_INCLUDE_PATH% ^
        /I %JAVA_INCLUDE_PATH%\win32 ^
        *.cpp

if errorlevel 1 (
        @echo:
        echo Compilation Failed !!!
        exit /b 1
)


@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries ...
echo Creating DLL ...
echo ----------------------------------------------------------------------------------------------------------------
link.exe ^
        *.obj ^
        /DLL ^
        user32.lib gdi32.lib ole32.lib oleaut32.lib


if errorlevel 1 (
        @echo:
        echo Linking Failed !!!
        exit /b 1
)
