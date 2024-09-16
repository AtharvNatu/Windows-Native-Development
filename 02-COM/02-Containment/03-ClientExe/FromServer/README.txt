Providing you with 3 files -

1. ClassFactoryDllServerWithRegFile.h
2. ClassFactoryDllServerWithRegFile.dll
3. ClassFactoryDllServerWithRegFile.reg

About Header File
-------------------------------------------------------------------------------
- Header file contains 2 interfaces -> ISum and ISubtract
- ISum provides SumOfTwoIntegers() functionality
- ISubtract provides SubtractionOfTwoIntegers() functionality
- Both interfaces inherit from IUnknown
- Both interfaces and required GUIDs are given in header file
- You can call above mentioned functions on respective interface pointers
-------------------------------------------------------------------------------

Building Client Application
-------------------------------------------------------------------------------
1. Copy the DLL to C:\Windows\System32
2. Run the Registry File(.reg) to register the DLL
3. Build and Run the client application
-------------------------------------------------------------------------------
