@echo off

cls

if exist *.obj del *.obj
if exist *.dll del *.dll
if exist *.exp del *.exp
if exist *.lib del *.lib

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
cl.exe /c /EHsc AggregationOuterComponentWithRegFile.cpp

@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries...
echo Creating Library...
echo ----------------------------------------------------------------------------------------------------------------
link.exe AggregationOuterComponentWithRegFile.obj /DLL /DEF:AggregationOuterComponentWithRegFile.def user32.lib ole32.lib
