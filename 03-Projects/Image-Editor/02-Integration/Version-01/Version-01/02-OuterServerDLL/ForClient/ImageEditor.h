#pragma once

class IColorInversion:public IUnknown
{
	public:
		// Pure Virtual Function
		virtual HRESULT __stdcall ColorInversion(COLORREF, COLORREF*) = 0;
};

// CLSID of ImageEditor Component -> {A0E209B2-7EEE-4C54-ADD3-ADFAFA46E9A1}
const CLSID CLSID_ImageEditor = 
{ 0xa0e209b2, 0x7eee, 0x4c54, 0xad, 0xd3, 0xad, 0xfa, 0xfa, 0x46, 0xe9, 0xa1 };

// IID of IColorInversion Interface -> {0B44BE1D-A3AE-404F-89AF-D86AAF4B5AFD}
const IID IID_IColorInversion = 
{ 0xb44be1d, 0xa3ae, 0x404f, 0x89, 0xaf, 0xd8, 0x6a, 0xaf, 0x4b, 0x5a, 0xfd };
