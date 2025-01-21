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

// CLSID of ImageToolkit Component -> {2F5B06BE-BF26-4AC1-B64F-5DE34974419D}
const CLSID CLSID_ImageToolkit =
{ 0x2f5b06be, 0xbf26, 0x4ac1, 0xb6, 0x4f, 0x5d, 0xe3, 0x49, 0x74, 0x41, 0x9d };

// IID of IDesaturation Interface -> {0C951FE0-84D0-4A7A-9712-14C743CB4A8A}
const IID IID_Desaturation =
{ 0xc951fe0, 0x84d0, 0x4a7a, 0x97, 0x12, 0x14, 0xc7, 0x43, 0xcb, 0x4a, 0x8a };

// IID of ISepia Interface -> {817DBFA8-8C34-4693-8FDB-AD69B499153}
const IID IID_ISepia =
{ 0x817dbfa8, 0x8c34, 0x4693, 0x8f, 0xdb, 0xad, 0x69, 0xb4, 0x99, 0x15, 0x33 };

