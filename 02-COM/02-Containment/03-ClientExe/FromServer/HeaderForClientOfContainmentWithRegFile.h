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

class IMultiply :public IUnknown
{
public:
	// Pure Virtual Function
	virtual HRESULT __stdcall MultiplicationOfTwoIntegers(int, int, int*) = 0;
};

class IDivide :public IUnknown
{
public:
	// Pure Virtual Function
	virtual HRESULT __stdcall DivisionOfTwoIntegers(int, int, int*) = 0;
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

// IID of IMultiply Interface -> {D02D3308-4428-413B-A10B-7C4958316E15}
const IID IID_IMultiply =
{ 0xd02d3308, 0x4428, 0x413b, 0xa1, 0xb, 0x7c, 0x49, 0x58, 0x31, 0x6e, 0x15 };

// IID of IDivide Interface -> {CD8786F0-17E0-4E5F-BB3E-9592FC264C3D}
const IID IID_IDivide =
{ 0xcd8786f0, 0x17e0, 0x4e5f, 0xbb, 0x3e, 0x95, 0x92, 0xfc, 0x26, 0x4c, 0x3d };

