@echo off

cls

cd Bin

if exist *.obj del *.obj
if exist *.exe del *.exe
if exist *.res del *.res

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
cl.exe /c /EHsc ..\Source\*.cpp

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
link.exe *.obj *.res user32.lib gdi32.lib comdlg32.lib ole32.lib Shlwapi.lib Advapi32.lib /SUBSYSTEM:WINDOWS /OUT:ImageEditor.exe

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Launching Application ...
echo ----------------------------------------------------------------------------------------------------------------
move ImageEditor.exe ..\  > nul 2>&1
cd ..
ImageEditor.exe

