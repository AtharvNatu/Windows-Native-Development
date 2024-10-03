// 2/10/2024 -> Changes in Aggregation Outer Component are w.r.t Containment Outer Component

#include <Windows.h>
#include "AggregationOuterComponentWithRegFile.h"
#include "AggregationInnerComponentWithRegFile.h"

// Class Declarations
class Calculator :public ISum, ISubtract
{
	private:
		long m_cRef;
		IUnknown* m_pIUnknownInner;

	public:
		// Constructor and Destructor Declarations
		Calculator(void);
		~Calculator(void);

		// Inherited IUnknown Method Declarations
		HRESULT __stdcall QueryInterface(REFIID, void**);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);

		// Inherited ISum Method Declarations
		HRESULT __stdcall SumOfTwoIntegers(int, int, int*);

		// Inherited ISubtract Method Declarations
		HRESULT __stdcall SubtractionOfTwoIntegers(int, int, int*);

		// Custom Method for Inner Component Creation
		HRESULT __stdcall InitializeInnerComponent(void);
};

class CalculatorClassFactory :public IClassFactory
{
	private:
		long m_cRef;

	public:
		// Constructor and Destructor Declarations
		CalculatorClassFactory(void);
		~CalculatorClassFactory(void);

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

// Calculator Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
Calculator::Calculator(void)
{
	// Code
	m_cRef = 1;
	m_pIUnknownInner = NULL;

	InterlockedIncrement(&glNumberOfActiveComponents);
}

// Destructor
Calculator::~Calculator(void)
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
HRESULT Calculator::QueryInterface(REFIID riid, void** ppv)
{
	// Code
	if (riid == IID_IUnknown)
		*ppv = static_cast<ISum*>(this);
	else if (riid == IID_ISum)
		*ppv = static_cast<ISum*>(this);
	else if (riid == IID_ISubtract)
		*ppv = static_cast<ISubtract*>(this);
	else if (riid == IID_IMultiply)
		return m_pIUnknownInner->QueryInterface(riid, ppv);
	else if (riid == IID_IDivide)
		return m_pIUnknownInner->QueryInterface(riid, ppv);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG Calculator::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG Calculator::Release(void)
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
HRESULT Calculator::SumOfTwoIntegers(int num1, int num2, int* pSum)
{
	// Code
	*pSum = num1 + num2;
	return S_OK;
}

// ISubtract's Methods
HRESULT Calculator::SubtractionOfTwoIntegers(int num1, int num2, int* pSubtract)
{
	// Code
	*pSubtract = num1 - num2;
	return S_OK;
}

HRESULT Calculator::InitializeInnerComponent(void)
{
	// Variable Declarations
	HRESULT hr = S_OK;

	// Code
	hr = CoCreateInstance(
			CLSID_MultiplyDivide, 
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

// CalculatorClassFactory Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CalculatorClassFactory::CalculatorClassFactory(void)
{
	// Code
	m_cRef = 1;
}

// Destructor
CalculatorClassFactory::~CalculatorClassFactory(void)
{
	// Code
}

// Inherited IUnknown Methods
HRESULT CalculatorClassFactory::QueryInterface(REFIID riid, void** ppv)
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

ULONG CalculatorClassFactory::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CalculatorClassFactory::Release(void)
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
HRESULT CalculatorClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void **ppv)
{
	// Variable Declarations
	Calculator* pCalculator = NULL;
	HRESULT hr = S_OK;

	// Code
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	pCalculator = new Calculator();
	if (pCalculator == NULL)
		return E_OUTOFMEMORY;

	// Initialize Inner Component
	hr = pCalculator->InitializeInnerComponent();
	if (FAILED(hr))
	{
		MessageBox(
			NULL,
			TEXT("Failed To Initialize Inner Component ... Exiting !!!"),
			TEXT("COM Error"),
			MB_ICONERROR | MB_OK
		);

		pCalculator->Release();

		return hr;
	}

	hr = pCalculator->QueryInterface(riid, ppv);
	pCalculator->Release();

	return hr;
}

HRESULT CalculatorClassFactory::LockServer(BOOL fLock)
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
	CalculatorClassFactory* pCalculatorClassFactory = NULL;
	HRESULT hr = S_OK;

	// Code
	if (rclsid != CLSID_SumSubtract)
		return CLASS_E_CLASSNOTAVAILABLE;

	pCalculatorClassFactory = new CalculatorClassFactory();
	if (pCalculatorClassFactory == NULL)
		return E_OUTOFMEMORY;
	
	hr = pCalculatorClassFactory->QueryInterface(riid, ppv);
	pCalculatorClassFactory->Release();

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
