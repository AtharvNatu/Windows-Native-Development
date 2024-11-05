@echo off

cls

if exist *.c del *.c
if exist *.h del *.h
if exist *.tlb del *.tlb

echo ----------------------------------------------------------------------------------------------------------------
echo Compiling MIDL Source ...
echo Generating Source Files and Type Library...
echo ----------------------------------------------------------------------------------------------------------------
midl.exe ^
	/h AutomationProxyStub.h ^
	/iid AutomationProxyStubGuids.c ^
	/dlldata AutomationProxyStubDllData.c ^
	/proxy AutomationProxyStub.c ^
	AutomationServerTypeLibrary.idl
