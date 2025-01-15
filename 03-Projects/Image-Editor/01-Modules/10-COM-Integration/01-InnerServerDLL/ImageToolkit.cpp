#include <Windows.h>
#include "ImageToolkit.h"

// Interface Declaration
interface INoAggregationIUnknown
{
	virtual HRESULT __stdcall QueryInterface_NoAggregation(REFIID, void**) = 0;
	virtual ULONG __stdcall AddRef_NoAggregation(void) = 0;
	virtual ULONG __stdcall Release_NoAggregation(void) = 0;
};

// Class Declarations
class CImageToolkit :public INoAggregationIUnknown, IDesaturation, ISepia
{
	private:
		long m_cRef;
		IUnknown* m_pIUnknownOuter;

	public:
		// Constructor and Destructor Declarations
		CImageToolkit(IUnknown*);
		~CImageToolkit(void);

		// Aggregation Non-Supported IUnknown Method Declarations
		HRESULT __stdcall QueryInterface_NoAggregation(REFIID, void**);
		ULONG __stdcall AddRef_NoAggregation(void);
		ULONG __stdcall Release_NoAggregation(void);

		// Inherited IUnknown Method Declarations
		HRESULT __stdcall QueryInterface(REFIID, void**);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);

		// Inherited IDestauration Method Declarations
		HRESULT __stdcall Desaturation(COLORREF, COLORREF*);

		// Inherited ISepia Method Declarations
		HRESULT __stdcall Sepia(COLORREF, COLORREF*);
};

class CImageToolkitClassFactory :public IClassFactory
{
	private:
		long m_cRef;

	public:
		// Constructor and Destructor Declarations
		CImageToolkitClassFactory(void);
		~CImageToolkitClassFactory(void);

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

// DllMain
BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	// Code
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;

	default:
		break;
	}

	return TRUE;
}

// CImageToolkit Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CImageToolkit::CImageToolkit(IUnknown *pIUnknownOuter)
{
	// Code
	m_cRef = 1;

	InterlockedIncrement(&glNumberOfActiveComponents);

	if (pIUnknownOuter != NULL)
		m_pIUnknownOuter = pIUnknownOuter;
	else
		m_pIUnknownOuter = reinterpret_cast<IUnknown*>(static_cast<INoAggregationIUnknown*>(this));
}

// Destructor
CImageToolkit::~CImageToolkit(void)
{
	// Code
	InterlockedDecrement(&glNumberOfActiveComponents);
}

// Aggregation Non-Supported IUnknown Methods
HRESULT CImageToolkit::QueryInterface_NoAggregation(REFIID riid, void** ppv)
{
	// Code
	if (riid == IID_IUnknown)
		*ppv = static_cast<INoAggregationIUnknown*>(this);
	else if (riid == IID_Desaturation)
		*ppv = static_cast<IDesaturation*>(this);
	else if (riid == IID_ISepia)
		*ppv = static_cast<ISepia*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG CImageToolkit::AddRef_NoAggregation(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CImageToolkit::Release_NoAggregation(void)
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

// Inherited IUnknown Methods
HRESULT CImageToolkit::QueryInterface(REFIID riid, void** ppv)
{
	// Code
	return m_pIUnknownOuter->QueryInterface(riid, ppv);
}

ULONG CImageToolkit::AddRef(void)
{
	// Code
	return m_pIUnknownOuter->AddRef();
}

ULONG CImageToolkit::Release(void)
{
	// Code
	return m_pIUnknownOuter->Release();
}

// IDesaturation's Methods
HRESULT CImageToolkit::Desaturation(COLORREF originalPixelColor, COLORREF* desaturatedPixelColor)
{
	// Code
	unsigned int originalR = GetRValue(originalPixelColor);
	unsigned int originalG = GetGValue(originalPixelColor);
	unsigned int originalB = GetBValue(originalPixelColor);

	unsigned int desaturatedR = (unsigned int)((float)originalR * 0.3f);
	unsigned int desaturatedG = (unsigned int)((float)originalG * 0.59f);
	unsigned int desaturatedB = (unsigned int)((float)originalB * 0.11f);

	unsigned int finalDesaturatedColor = desaturatedR + desaturatedG + desaturatedB;

	*desaturatedPixelColor = RGB(finalDesaturatedColor, finalDesaturatedColor, finalDesaturatedColor);

	return S_OK;
}

// ISepia's Methods
HRESULT CImageToolkit::Sepia(COLORREF originalPixelColor, COLORREF* sepiaPixelColor)
{
	// Code
	unsigned int originalR = GetRValue(originalPixelColor);
	unsigned int originalG = GetGValue(originalPixelColor);
	unsigned int originalB = GetBValue(originalPixelColor);

	unsigned int sepiaR = (unsigned int)(((float)originalR * 0.393f) + ((float)originalG * 0.769f) + ((float)originalB * 0.189f));
	if (sepiaR > 255)
		sepiaR = 255;
	
	unsigned int sepiaG = (unsigned int)(((float)originalR * 0.349f) + ((float)originalG * 0.686f) + ((float)originalB * 0.168f));
	if (sepiaG > 255)
		sepiaG = 255;

	unsigned int sepiaB = (unsigned int)(((float)originalR * 0.272f) + ((float)originalG * 0.534f) + ((float)originalB * 0.131f));
	if (sepiaB > 255)
		sepiaB = 255;

	*sepiaPixelColor = RGB(sepiaR, sepiaG, sepiaB);

	return S_OK;
}
//-----------------------------------------------------------------------------------------------------

// CImageToolkitClassFactory Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CImageToolkitClassFactory::CImageToolkitClassFactory(void)
{
	// Code
	m_cRef = 1;
}

// Destructor
CImageToolkitClassFactory::~CImageToolkitClassFactory(void)
{
	// Code
}

// Inherited IUnknown Methods
HRESULT CImageToolkitClassFactory::QueryInterface(REFIID riid, void** ppv)
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

ULONG CImageToolkitClassFactory::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CImageToolkitClassFactory::Release(void)
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
HRESULT CImageToolkitClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void **ppv)
{
	// Variable Declarations
	CImageToolkit* pCImageToolkit = NULL;
	HRESULT hr = S_OK;

	// Code
	if ((pUnkOuter != NULL) && (riid != IID_IUnknown))
		return CLASS_E_NOAGGREGATION;

	pCImageToolkit = new CImageToolkit(pUnkOuter);
	if (pCImageToolkit == NULL)
		return E_OUTOFMEMORY;

	hr = pCImageToolkit->QueryInterface_NoAggregation(riid, ppv);

	pCImageToolkit->Release_NoAggregation();

	return hr;
}

HRESULT CImageToolkitClassFactory::LockServer(BOOL fLock)
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
	CImageToolkitClassFactory* pCImageToolkitClassFactory = NULL;
	HRESULT hr = S_OK;

	// Code
	if (rclsid != CLSID_ImageToolkit)
		return CLASS_E_CLASSNOTAVAILABLE;

	pCImageToolkitClassFactory = new CImageToolkitClassFactory();
	if (pCImageToolkitClassFactory == NULL)
		return E_OUTOFMEMORY;
	
	hr = pCImageToolkitClassFactory->QueryInterface(riid, ppv);

	pCImageToolkitClassFactory->Release();

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
//-----------------------------------------------------------------------------------------------------
