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

// CLSID of MultiplyDivide Component -> // {37FE8C34-1CF1-43EC-9D26-9FC496CFE54B}
const CLSID CLSID_MultiplyDivide =
{ 0x37fe8c34, 0x1cf1, 0x43ec, 0x9d, 0x26, 0x9f, 0xc4, 0x96, 0xcf, 0xe5, 0x4b };

// IID of IMultiply Interface -> {D02D3308-4428-413B-A10B-7C4958316E15}
const IID IID_IMultiply = 
{ 0xd02d3308, 0x4428, 0x413b, 0xa1, 0xb, 0x7c, 0x49, 0x58, 0x31, 0x6e, 0x15 };

// IID of IDivide Interface -> {CD8786F0-17E0-4E5F-BB3E-9592FC264C3D}
const IID IID_IDivide = 
{ 0xcd8786f0, 0x17e0, 0x4e5f, 0xbb, 0x3e, 0x95, 0x92, 0xfc, 0x26, 0x4c, 0x3d };

