#pragma once

class ISum:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall SumOfTwoIntegers(int, int, int*) = 0;
};

class ISubtract:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall SubtractionOfTwoIntegers(int, int, int*) = 0;
};

// CLSID of SumSubtract Component -> {321E5953-3F44-4109-B36A-CB465AFAC4BF}
const CLSID CLSID_SumSubtract = 
{ 0x321e5953, 0x3f44, 0x4109, 0xb3, 0x6a, 0xcb, 0x46, 0x5a, 0xfa, 0xc4, 0xbf };

// IID of ISum Interface -> {F7E3ECBE-DFD1-417C-80D6-FB3D8B7596CF}
const IID IID_ISum = 
{ 0xf7e3ecbe, 0xdfd1, 0x417c, 0x80, 0xd6, 0xfb, 0x3d, 0x8b, 0x75, 0x96, 0xcf };

// IID of ISubtract Interface -> {A4B9A6C6-7EF6-4112-A21C-D3C05638AC57}
const IID IID_ISubtract = 
{ 0xa4b9a6c6, 0x7ef6, 0x4112, 0xa2, 0x1c, 0xd3, 0xc0, 0x56, 0x38, 0xac, 0x57 };

