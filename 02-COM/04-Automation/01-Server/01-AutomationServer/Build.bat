@echo off

cls

if exist *.obj del *.obj
if exist *.dll del *.dll
if exist *.exp del *.exp
if exist *.lib del *.lib

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
cl.exe /c /EHsc /D UNICODE AutomationServer.cpp

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries...
echo Creating Library...
echo ----------------------------------------------------------------------------------------------------------------
link.exe AutomationServer.obj /DLL /DEF:AutomationServer.def user32.lib oleaut32.lib
