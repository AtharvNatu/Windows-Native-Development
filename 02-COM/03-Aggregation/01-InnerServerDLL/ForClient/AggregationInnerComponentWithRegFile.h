#pragma once

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

// CLSID of MultiplyDivide Component -> {654D2D38-73B6-44E5-9F15-BF3B883C1A08}
const CLSID CLSID_MultiplyDivide =
{ 0x654d2d38, 0x73b6, 0x44e5, 0x9f, 0x15, 0xbf, 0x3b, 0x88, 0x3c, 0x1a, 0x8 };

// IID of IMultiply Interface -> {91841CFB-FD57-40BA-B7A3-1C3C1DBE3F84}
const IID IID_IMultiply =
{ 0x91841cfb, 0xfd57, 0x40ba, 0xb7, 0xa3, 0x1c, 0x3c, 0x1d, 0xbe, 0x3f, 0x84 };

// IID of IDivide Interface -> {E68C6C9D-45E3-4F88-8E4D-9E703A11924D}
const IID IID_IDivide =
{ 0xe68c6c9d, 0x45e3, 0x4f88, 0x8e, 0x4d, 0x9e, 0x70, 0x3a, 0x11, 0x92, 0x4d };

