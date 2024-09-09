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

// CLSID of SumSubtract Component -> {95A50990-1617-4C1E-8462-47A523891FF3}
const CLSID CLSID_SumSubtract = 
{ 0x95a50990, 0x1617, 0x4c1e, 0x84, 0x62, 0x47, 0xa5, 0x23, 0x89, 0x1f, 0xf3 };

// IID of ISum Interface -> {6B887E25-C00B-4CFA-A900-DB896DB6A992}
const IID IID_ISum = 
{ 0x6b887e25, 0xc00b, 0x4cfa, 0xa9, 0x0, 0xdb, 0x89, 0x6d, 0xb6, 0xa9, 0x92 };

// IID of ISubtract Interface -> {B9A52F34-9B8E-4FF7-A067-523F0FC541AD}
const IID IID_ISubtract = 
{ 0xb9a52f34, 0x9b8e, 0x4ff7, 0xa0, 0x67, 0x52, 0x3f, 0xf, 0xc5, 0x41, 0xad };

