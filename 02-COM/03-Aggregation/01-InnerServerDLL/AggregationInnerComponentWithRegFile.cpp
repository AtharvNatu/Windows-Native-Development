// 2/10/2024 -> Changes in Aggregation Inner Component are w.r.t Containment Inner Component

#include <Windows.h>
#include "AggregationInnerComponentWithRegFile.h"

// Interface Declaration
interface INoAggregationIUnknown
{
	virtual HRESULT __stdcall QueryInterface_NoAggregation(REFIID, void**) = 0;
	virtual ULONG __stdcall AddRef_NoAggregation(void) = 0;
	virtual ULONG __stdcall Release_NoAggregation(void) = 0;
};

// Class Declarations
class CMultiplyDivide :public INoAggregationIUnknown, IMultiply, IDivide
{
	private:
		long m_cRef;
		IUnknown* m_pIUnknownOuter;

	public:
		// Constructor and Destructor Declarations
		CMultiplyDivide(IUnknown*);
		~CMultiplyDivide(void);

		// Aggregation Non-Supported IUnknown Method Declarations
		HRESULT __stdcall QueryInterface_NoAggregation(REFIID, void**);
		ULONG __stdcall AddRef_NoAggregation(void);
		ULONG __stdcall Release_NoAggregation(void);

		// Inherited IUnknown Method Declarations
		HRESULT __stdcall QueryInterface(REFIID, void**);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);

		// Inherited IMultiply Method Declarations
		HRESULT __stdcall MultiplicationOfTwoIntegers(int, int, int*);

		// Inherited IDivide Method Declarations
		HRESULT __stdcall DivisionOfTwoIntegers(int, int, int*);
};

class CMultiplyDivideClassFactory :public IClassFactory
{
	private:
		long m_cRef;

	public:
		// Constructor and Destructor Declarations
		CMultiplyDivideClassFactory(void);
		~CMultiplyDivideClassFactory(void);

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

// CMultiplyDivide Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CMultiplyDivide::CMultiplyDivide(IUnknown *pIUnknownOuter)
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
CMultiplyDivide::~CMultiplyDivide(void)
{
	// Code
	InterlockedDecrement(&glNumberOfActiveComponents);
}

// Aggregation Non-Supported IUnknown Methods
HRESULT CMultiplyDivide::QueryInterface_NoAggregation(REFIID riid, void** ppv)
{
	// Code
	if (riid == IID_IUnknown)
		*ppv = static_cast<INoAggregationIUnknown*>(this);
	else if (riid == IID_IMultiply)
		*ppv = static_cast<IMultiply*>(this);
	else if (riid == IID_IDivide)
		*ppv = static_cast<IDivide*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG CMultiplyDivide::AddRef_NoAggregation(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CMultiplyDivide::Release_NoAggregation(void)
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
HRESULT CMultiplyDivide::QueryInterface(REFIID riid, void** ppv)
{
	// Code
	return m_pIUnknownOuter->QueryInterface(riid, ppv);
}

ULONG CMultiplyDivide::AddRef(void)
{
	// Code
	return m_pIUnknownOuter->AddRef();
}

ULONG CMultiplyDivide::Release(void)
{
	// Code
	return m_pIUnknownOuter->Release();
}

// IMultiply's Methods
HRESULT CMultiplyDivide::MultiplicationOfTwoIntegers(int num1, int num2, int* pMul)
{
	// Code
	*pMul = num1 * num2;
	return S_OK;
}

// IDivide's Methods
HRESULT CMultiplyDivide::DivisionOfTwoIntegers(int num1, int num2, int* pDivide)
{
	// Code
	*pDivide = num1 / num2;
	return S_OK;
}
//-----------------------------------------------------------------------------------------------------

// CMultiplyDivideClassFactory Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CMultiplyDivideClassFactory::CMultiplyDivideClassFactory(void)
{
	// Code
	m_cRef = 1;
}

// Destructor
CMultiplyDivideClassFactory::~CMultiplyDivideClassFactory(void)
{
	// Code
}

// Inherited IUnknown Methods
HRESULT CMultiplyDivideClassFactory::QueryInterface(REFIID riid, void** ppv)
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

ULONG CMultiplyDivideClassFactory::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CMultiplyDivideClassFactory::Release(void)
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
HRESULT CMultiplyDivideClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void **ppv)
{
	// Variable Declarations
	CMultiplyDivide* pCMultiplyDivide = NULL;
	HRESULT hr = S_OK;

	// Code
	if ((pUnkOuter != NULL) && (riid != IID_IUnknown))
		return CLASS_E_NOAGGREGATION;

	pCMultiplyDivide = new CMultiplyDivide(pUnkOuter);
	if (pCMultiplyDivide == NULL)
		return E_OUTOFMEMORY;

	hr = pCMultiplyDivide->QueryInterface_NoAggregation(riid, ppv);

	pCMultiplyDivide->Release_NoAggregation();

	return hr;
}

HRESULT CMultiplyDivideClassFactory::LockServer(BOOL fLock)
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
	CMultiplyDivideClassFactory* pCMultiplyDivideClassFactory = NULL;
	HRESULT hr = S_OK;

	// Code
	if (rclsid != CLSID_MultiplyDivide)
		return CLASS_E_CLASSNOTAVAILABLE;

	pCMultiplyDivideClassFactory = new CMultiplyDivideClassFactory();
	if (pCMultiplyDivideClassFactory == NULL)
		return E_OUTOFMEMORY;
	
	hr = pCMultiplyDivideClassFactory->QueryInterface(riid, ppv);

	pCMultiplyDivideClassFactory->Release();

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
