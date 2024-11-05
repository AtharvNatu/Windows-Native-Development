#pragma once

class IMyMath : public IDispatch
{
	public:
		// Pure Virtual Functions
		virtual HRESULT __stdcall SumOfTwoIntegers(int, int, int*) = 0;
		virtual HRESULT __stdcall SubtractionOfTwoIntegers(int, int, int*) = 0;
};

// CLSID of MyMath Component -> {69DEFCBC-00E1-4817-A8D1-C608EE5C20F3}
const CLSID CLSID_MyMath = 
{ 0x69defcbc, 0xe1, 0x4817, 0xa8, 0xd1, 0xc6, 0x8, 0xee, 0x5c, 0x20, 0xf3 };

// IID of IMyMath Interface -> {355AC00C-6087-473D-865F-9008CF098DD9}
const IID IID_IMyMath =
{ 0x355ac00c, 0x6087, 0x473d, 0x86, 0x5f, 0x90, 0x8, 0xcf, 0x9, 0x8d, 0xd9 };


