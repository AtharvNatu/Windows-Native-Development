#include <Windows.h>
#include "ClassFactoryDllServerWithRegFile.h"

// Class Declarations
class CSumSubtract :public ISum, ISubtract
{
	private:
		long m_cRef;

	public:
		// Constructor and Destructor Declarations
		CSumSubtract(void);
		~CSumSubtract(void);

		// Inherited IUnknown Method Declarations
		HRESULT __stdcall QueryInterface(REFIID, void**);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);

		// Inherited ISum Method Declarations
		HRESULT __stdcall SumOfTwoIntegers(int, int, int*);

		// Inherited ISubtract Method Declarations
		HRESULT __stdcall SubtractionOfTwoIntegers(int, int, int*);
};

class CSumSubtractClassFactory :public IClassFactory
{
	private:
		long m_cRef;

	public:
		// Constructor and Destructor Declarations
		CSumSubtractClassFactory(void);
		~CSumSubtractClassFactory(void);

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

// CSumSubtract Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CSumSubtract::CSumSubtract(void)
{
	// Code
	m_cRef = 1;
	InterlockedIncrement(&glNumberOfActiveComponents);
}

// Destructor
CSumSubtract::~CSumSubtract(void)
{
	// Code
	InterlockedDecrement(&glNumberOfActiveComponents);
}

// Inherited IUnknown Methods
HRESULT CSumSubtract::QueryInterface(REFIID riid, void** ppv)
{
	// Code
	if (riid == IID_IUnknown)
		*ppv = static_cast<ISum*>(this);
	else if (riid == IID_ISum)
		*ppv = static_cast<ISum*>(this);
	else if (riid == IID_ISubtract)
		*ppv = static_cast<ISubtract*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG CSumSubtract::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CSumSubtract::Release(void)
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

// ISum's Methods
HRESULT CSumSubtract::SumOfTwoIntegers(int num1, int num2, int* pSum)
{
	// Code
	*pSum = num1 + num2;
	return S_OK;
}

// ISubtract's Methods
HRESULT CSumSubtract::SubtractionOfTwoIntegers(int num1, int num2, int* pSubtract)
{
	// Code
	*pSubtract = num1 - num2;
	return S_OK;
}
//-----------------------------------------------------------------------------------------------------

// CSumSubtractClassFactory Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CSumSubtractClassFactory::CSumSubtractClassFactory(void)
{
	// Code
	m_cRef = 1;
}

// Destructor
CSumSubtractClassFactory::~CSumSubtractClassFactory(void)
{
	// Code
}

// Inherited IUnknown Methods
HRESULT CSumSubtractClassFactory::QueryInterface(REFIID riid, void** ppv)
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

ULONG CSumSubtractClassFactory::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CSumSubtractClassFactory::Release(void)
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
HRESULT CSumSubtractClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void **ppv)
{
	// Variable Declarations
	CSumSubtract* pCSumSubtract = NULL;
	HRESULT hr = S_OK;

	// Code
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	pCSumSubtract = new CSumSubtract();
	if (pCSumSubtract == NULL)
		return E_OUTOFMEMORY;

	hr = pCSumSubtract->QueryInterface(riid, ppv);
	pCSumSubtract->Release();

	return hr;
}

HRESULT CSumSubtractClassFactory::LockServer(BOOL fLock)
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
	CSumSubtractClassFactory* pCSumSubtractClassFactory = NULL;
	HRESULT hr = S_OK;

	// Code
	if (rclsid != CLSID_SumSubtract)
		return CLASS_E_CLASSNOTAVAILABLE;

	pCSumSubtractClassFactory = new CSumSubtractClassFactory();
	if (pCSumSubtractClassFactory == NULL)
		return E_OUTOFMEMORY;
	
	hr = pCSumSubtractClassFactory->QueryInterface(riid, ppv);
	pCSumSubtractClassFactory->Release();

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
