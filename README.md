
# Windows-Native-Development
Windows OS Specific Development using Win32 SDK, COM and WinRT

# For Win32

To Build the Source Code using x64 Native Tools Command Prompt

1. Open the Command Prompt in the source code directory
2.	To Compile C/C++ Code, use the following command
		cl.exe /c /EHsc Source.cpp
3. To Link the code with external libraries, use the following command
		link.exe Source.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
4. These steps will create the executable application

You can find a .bat file for every code in respective directories which automates the above build process

