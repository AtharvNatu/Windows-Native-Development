@echo off

cls

if exist *.obj del *.obj
if exist *.dll del *.dll
if exist *.exp del *.exp
if exist *.lib del *.lib

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling Source Code ...
echo ----------------------------------------------------------------------------------------------------------------
cl.exe /c /EHsc /D REGISTER_PROXY_DLL ^
	AutomationProxyStub.c ^
	AutomationProxyStubGuids.c ^
	AutomationProxyStubDllData.c


@echo:
echo ----------------------------------------------------------------------------------------------------------------
echo Linking Libraries...
echo Creating Library...
echo ----------------------------------------------------------------------------------------------------------------
link.exe ^
	AutomationProxyStub.obj ^
	AutomationProxyStubGuids.obj ^
	AutomationProxyStubDllData.obj ^
	/DLL /DEF:AutomationProxyStub.def ^
	user32.lib ole32.lib oleaut32.lib advapi32.lib rpcns4.lib rpcrt4.lib ^
	/SUBSYSTEM:WINDOWS
