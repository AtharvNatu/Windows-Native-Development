#include <Windows.h>
#include "ImageEditor.h"
#include "ImageToolkit.h"

// Class Declarations
class ImageEditor :public IColorInversion
{
	private:
		long m_cRef;
		IUnknown* m_pIUnknownInner;

	public:
		// Constructor and Destructor Declarations
		ImageEditor(void);
		~ImageEditor(void);

		// Inherited IUnknown Method Declarations
		HRESULT __stdcall QueryInterface(REFIID, void**);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);

		// Inherited ISum Method Declarations
		HRESULT __stdcall ColorInversion(COLORREF, COLORREF*);

		// Custom Method for Inner Component Creation
		HRESULT __stdcall InitializeInnerComponent(void);
};

class ImageEditorClassFactory :public IClassFactory
{
	private:
		long m_cRef;

	public:
		// Constructor and Destructor Declarations
		ImageEditorClassFactory(void);
		~ImageEditorClassFactory(void);

		// IUnknown(Inherited) Method Declarations
		HRESULT __stdcall QueryInterface(REFIID, void**);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);

		// IClassFactory(Inherited) Method Declarations
		HRESULT __stdcall CreateInstance(IUnknown*, REFIID, void**);
		HRESULT __stdcall LockServer(BOOL);
};

// Global Variable Declarations
long glNumberOfActiveComponents = 0;
long glNumberofServerLocks = 0;
HMODULE ghModule = NULL;

// DllMain
BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	// Code
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			ghModule = hDll;
		break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			if (ghModule)
				ghModule = NULL;
		break;

		default:
			break;
	}

	return TRUE;
}

// ImageEditor Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
ImageEditor::ImageEditor(void)
{
	// Code
	m_cRef = 1;
	m_pIUnknownInner = NULL;

	InterlockedIncrement(&glNumberOfActiveComponents);
}

// Destructor
ImageEditor::~ImageEditor(void)
{
	// Code
	InterlockedDecrement(&glNumberOfActiveComponents);

	if (m_pIUnknownInner)
	{
		m_pIUnknownInner->Release();
		m_pIUnknownInner = NULL;
	}
}

// Inherited IUnknown Methods
HRESULT ImageEditor::QueryInterface(REFIID riid, void** ppv)
{
	// Code
	if (riid == IID_IUnknown)
		*ppv = static_cast<IColorInversion*>(this);
	else if (riid == IID_IColorInversion)
		*ppv = static_cast<IColorInversion*>(this);
	else if (riid == IID_Desaturation)
		return m_pIUnknownInner->QueryInterface(riid, ppv);
	else if (riid == IID_ISepia)
		return m_pIUnknownInner->QueryInterface(riid, ppv);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG ImageEditor::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG ImageEditor::Release(void)
{
	// Code
	InterlockedDecrement(&m_cRef);
	if (m_cRef == 0)
	{
		delete(this);
		return 0;
	}

	return m_cRef;
}

// IColorInversion's Methods
HRESULT ImageEditor::ColorInversion(COLORREF originalPixelColor, COLORREF* negativePixelColor)
{
	// Code
	unsigned int originalR = GetRValue(originalPixelColor);
	unsigned int originalG = GetGValue(originalPixelColor);
	unsigned int originalB = GetBValue(originalPixelColor);

	unsigned int negativeR = 255 - originalR;
	if (negativeR < 0)
		negativeR = 0;
	
	unsigned int negativeG = 255 - originalG;
	if (negativeG < 0)
		negativeG = 0;

	unsigned int negativeB = 255 - originalB;
	if (negativeB < 0)
		negativeB = 0;

	*negativePixelColor = RGB(negativeR, negativeG, negativeB);

	return S_OK;
}

HRESULT ImageEditor::InitializeInnerComponent(void)
{
	// Variable Declarations
	HRESULT hr = S_OK;

	// Code
	hr = CoCreateInstance(
			CLSID_ImageToolkit, 
			reinterpret_cast<IUnknown*>(this),	//* (this) points to the instance of Outer Component's CoClass and typecasted to IUnknown of the Outer Component
			CLSCTX_INPROC_SERVER, 
			IID_IUnknown, 
			(void**)&m_pIUnknownInner	//* Asking for IUnknown from Inner Component, but gets INoAggregationIUnknown
		);
	if (FAILED(hr))
	{
		MessageBox(
			NULL,
			TEXT("Inner IUnknown Interface Could Not Be Obtained ... Exiting !!!"),
			TEXT("COM Error"),
			MB_OK | MB_ICONERROR
		);
		return E_FAIL;
	}

	return S_OK;
}
//-----------------------------------------------------------------------------------------------------

// ImageEditorClassFactory Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
ImageEditorClassFactory::ImageEditorClassFactory(void)
{
	// Code
	m_cRef = 1;
}

// Destructor
ImageEditorClassFactory::~ImageEditorClassFactory(void)
{
	// Code
}

// Inherited IUnknown Methods
HRESULT ImageEditorClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	// Code
	if (riid == IID_IUnknown)
		*ppv = static_cast<IClassFactory*>(this);
	else if (riid == IID_IClassFactory)
		*ppv = static_cast<IClassFactory*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG ImageEditorClassFactory::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG ImageEditorClassFactory::Release(void)
{
	// Code
	InterlockedDecrement(&m_cRef);
	if (m_cRef == 0)
	{
		delete(this);
		return 0;
	}

	return m_cRef;
}

// IClassFactory's Method
HRESULT ImageEditorClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void **ppv)
{
	// Variable Declarations
	ImageEditor* pImageEditor = NULL;
	HRESULT hr = S_OK;

	// Code
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	pImageEditor = new ImageEditor();
	if (pImageEditor == NULL)
		return E_OUTOFMEMORY;

	// Initialize Inner Component
	hr = pImageEditor->InitializeInnerComponent();
	if (FAILED(hr))
	{
		MessageBox(
			NULL,
			TEXT("Failed To Initialize Inner Component ... Exiting !!!"),
			TEXT("COM Error"),
			MB_ICONERROR | MB_OK
		);

		pImageEditor->Release();

		return hr;
	}

	hr = pImageEditor->QueryInterface(riid, ppv);
	pImageEditor->Release();

	return hr;
}

HRESULT ImageEditorClassFactory::LockServer(BOOL fLock)
{
	// Code
	if (fLock)
		InterlockedIncrement(&glNumberofServerLocks);
	else
		InterlockedDecrement(&glNumberofServerLocks);

	return S_OK;
}

// DLL Export Functions
extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
	// Variable Declarations
	ImageEditorClassFactory* pImageEditorClassFactory = NULL;
	HRESULT hr = S_OK;

	// Code
	if (rclsid != CLSID_ImageEditor)
		return CLASS_E_CLASSNOTAVAILABLE;

	pImageEditorClassFactory = new ImageEditorClassFactory();
	if (pImageEditorClassFactory == NULL)
		return E_OUTOFMEMORY;
	
	hr = pImageEditorClassFactory->QueryInterface(riid, ppv);
	pImageEditorClassFactory->Release();

	return hr;
}

extern "C" HRESULT __stdcall DllCanUnloadNow(void)
{
	// Code
	if ((glNumberOfActiveComponents == 0) && (glNumberofServerLocks == 0))
		return S_OK;
	else
		return S_FALSE;
}

extern "C" HRESULT __stdcall DllRegisterServer(void)
{
	// Variable Declarations
	HKEY hCLSIDKey = NULL, hInProcServerKey = NULL;
	LONG lRet;
	TCHAR szModulePath[MAX_PATH];
	TCHAR szClassDescription[] = TEXT("Image Editor Component");
	TCHAR szThreadingModel[] = TEXT("Apartment");

	// Code
	__try
	{
		lRet = RegCreateKeyEx(
			HKEY_CLASSES_ROOT, 
			TEXT("CLSID\\{A0E209B2-7EEE-4C54-ADD3-ADFAFA46E9A1}"), 
			0, 
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
			NULL,
			&hCLSIDKey,
			NULL
		);
		if (lRet != ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(lRet);

		lRet = RegSetValueEx(
			hCLSIDKey,
			NULL,
			0,
			REG_SZ,
			(const BYTE*)szClassDescription,
			sizeof(szClassDescription)
		);
		if (lRet != ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(lRet);

		lRet = RegCreateKeyEx(
			hCLSIDKey, 
			TEXT("InProcServer32"), 
			0, 
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_SET_VALUE,
			NULL,
			&hInProcServerKey,
			NULL
		);
		if (lRet != ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(lRet);

		GetModuleFileName(ghModule, szModulePath, MAX_PATH);

		lRet = RegSetValueEx(
			hInProcServerKey,
			NULL,
			0,
			REG_SZ,
			(const BYTE*)szModulePath,
			sizeof(TCHAR) * (lstrlen(szModulePath) + 1)
		);
		if (lRet != ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(lRet);

		lRet = RegSetValueEx(
			hInProcServerKey,
			TEXT("ThreadingModel"),
			0,
			REG_SZ,
			(const BYTE*)szThreadingModel,
			sizeof(szThreadingModel)
		);
		if (lRet != ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(lRet);
	}
	__finally
	{
		if (hInProcServerKey)
			RegCloseKey(hInProcServerKey);

		if (hCLSIDKey)
			RegCloseKey(hCLSIDKey);
	}

	return S_OK;

}

extern "C" HRESULT __stdcall DllUnregisterServer(void)
{
	// Code
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\{A0E209B2-7EEE-4C54-ADD3-ADFAFA46E9A1}\\InProcServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\{A0E209B2-7EEE-4C54-ADD3-ADFAFA46E9A1}"));

	return S_OK;
}

//-----------------------------------------------------------------------------------------------------
