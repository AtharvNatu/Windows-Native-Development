#pragma once

class ISum :public IUnknown
{
public:
	// Pure Virtual Function
	virtual HRESULT __stdcall SumOfTwoIntegers(int, int, int*) = 0;
};

class ISubtract :public IUnknown
{
public:
	// Pure Virtual Function
	virtual HRESULT __stdcall SubtractionOfTwoIntegers(int, int, int*) = 0;
};

class IMultiply:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall MultiplicationOfTwoIntegers(int, int, int*) = 0;
};

class IDivide:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall DivisionOfTwoIntegers(int, int, int*) = 0;
};

// CLSID of SumSubtract Component -> {12C5EEE6-A492-4FFF-A5B1-3E1FB13738DD}
const CLSID CLSID_SumSubtract =
{ 0x12c5eee6, 0xa492, 0x4fff, 0xa5, 0xb1, 0x3e, 0x1f, 0xb1, 0x37, 0x38, 0xdd };

// IID of ISum Interface -> {CD729834-08A7-455A-8E22-09C91F87BA40}
const IID IID_ISum =
{ 0xcd729834, 0x8a7, 0x455a, 0x8e, 0x22, 0x9, 0xc9, 0x1f, 0x87, 0xba, 0x40 };

// IID of ISubtract Interface -> // {ED30CC1E-0E84-4603-8991-CBBF64A067C9}
const IID IID_ISubtract =
{ 0xed30cc1e, 0xe84, 0x4603, 0x89, 0x91, 0xcb, 0xbf, 0x64, 0xa0, 0x67, 0xc9 };

// IID of IMultiply Interface -> {91841CFB-FD57-40BA-B7A3-1C3C1DBE3F84}
const IID IID_IMultiply =
{ 0x91841cfb, 0xfd57, 0x40ba, 0xb7, 0xa3, 0x1c, 0x3c, 0x1d, 0xbe, 0x3f, 0x84 };

// IID of IDivide Interface -> {E68C6C9D-45E3-4F88-8E4D-9E703A11924D}
const IID IID_IDivide =
{ 0xe68c6c9d, 0x45e3, 0x4f88, 0x8e, 0x4d, 0x9e, 0x70, 0x3a, 0x11, 0x92, 0x4d };

