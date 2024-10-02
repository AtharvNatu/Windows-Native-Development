@echo off

cls

if exist *.obj del *.obj
if exist *.dll del *.dll
if exist *.exp del *.exp
if exist *.lib del *.lib

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
cl.exe /c /EHsc AggregationInnerComponentWithRegFile.cpp

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries...
echo Creating Library...
echo ----------------------------------------------------------------------------------------------------------------
link.exe AggregationInnerComponentWithRegFile.obj /DLL /DEF:AggregationInnerComponentWithRegFile.def user32.lib
