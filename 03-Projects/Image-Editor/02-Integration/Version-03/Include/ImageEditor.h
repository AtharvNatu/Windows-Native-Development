#pragma once

class IDesaturation:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall Desaturation(COLORREF, COLORREF*) = 0;
};

class ISepia:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall Sepia(COLORREF, COLORREF*) = 0;
};

class IColorInversion:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall ColorInversion(COLORREF, COLORREF*) = 0;
};


// CLSID of ImageEditor Component -> {A0E209B2-7EEE-4C54-ADD3-ADFAFA46E9A1}
const CLSID CLSID_ImageEditor = 
{ 0xa0e209b2, 0x7eee, 0x4c54, 0xad, 0xd3, 0xad, 0xfa, 0xfa, 0x46, 0xe9, 0xa1 };

// IID of IDesaturation Interface -> {0C951FE0-84D0-4A7A-9712-14C743CB4A8A}
const IID IID_Desaturation =
{ 0xc951fe0, 0x84d0, 0x4a7a, 0x97, 0x12, 0x14, 0xc7, 0x43, 0xcb, 0x4a, 0x8a };

// IID of ISepia Interface -> {817DBFA8-8C34-4693-8FDB-AD69B499153}
const IID IID_ISepia =
{ 0x817dbfa8, 0x8c34, 0x4693, 0x8f, 0xdb, 0xad, 0x69, 0xb4, 0x99, 0x15, 0x33 };

// IID of IColorInversion Interface -> {0B44BE1D-A3AE-404F-89AF-D86AAF4B5AFD}
const IID IID_IColorInversion = 
{ 0xb44be1d, 0xa3ae, 0x404f, 0x89, 0xaf, 0xd8, 0x6a, 0xaf, 0x4b, 0x5a, 0xfd };
