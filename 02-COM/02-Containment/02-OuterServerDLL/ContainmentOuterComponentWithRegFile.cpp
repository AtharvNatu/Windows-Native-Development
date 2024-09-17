#include <Windows.h>
#include "ContainmentOuterComponentWithRegFile.h"
#include "ContainmentInnerComponentWithRegFile.h"

// Class Declarations
class Calculator :public ISum, ISubtract, IMultiply, IDivide
{
	private:
		long m_cRef;
		IMultiply* m_pIMultiply;
		IDivide* m_pIDivide;

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

		// Inherited IMultiply Method Declarations
		HRESULT __stdcall MultiplicationOfTwoIntegers(int, int, int*);
		
		// Inherited IDivide Method Declarations
		HRESULT __stdcall DivisionOfTwoIntegers(int, int, int*);

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
	m_pIMultiply = NULL;
	m_pIDivide = NULL;

	InterlockedIncrement(&glNumberOfActiveComponents);
}

// Destructor
Calculator::~Calculator(void)
{
	// Code
	InterlockedDecrement(&glNumberOfActiveComponents);

	// Safe Release
	if (m_pIDivide)
	{
		m_pIDivide->Release();
		m_pIDivide = NULL;
	}

	if (m_pIMultiply)
	{
		m_pIMultiply->Release();
		m_pIMultiply = NULL;
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

// IMultiply's Methods
HRESULT Calculator::MultiplicationOfTwoIntegers(int num1, int num2, int* pMultiplication)
{
	// Code
	m_pIMultiply->MultiplicationOfTwoIntegers(num1, num2, pMultiplication);
	return S_OK;
}

// IDivide's Methods
HRESULT Calculator::DivisionOfTwoIntegers(int num1, int num2, int* pDivision)
{
	// Code
	m_pIDivide->DivisionOfTwoIntegers(num1, num2, pDivision);
	return S_OK;
}

HRESULT Calculator::InitializeInnerComponent(void)
{
	// Variable Declarations
	HRESULT hr = S_OK;

	// Code
	hr = CoCreateInstance(
			CLSID_MultiplyDivide, 
			NULL, 
			CLSCTX_INPROC_SERVER, 
			IID_IMultiply, 
			(void**)&m_pIMultiply
		);
	if (FAILED(hr))
	{
		MessageBox(
			NULL,
			TEXT("IMultiply Interface Could Not Be Obtained ... Exiting !!!"),
			TEXT("COM Error"),
			MB_OK | MB_ICONERROR
		);
		return E_FAIL;
	}

	hr = m_pIMultiply->QueryInterface(IID_IDivide, (void**)&m_pIDivide);
	if (FAILED(hr))
	{
		MessageBox(
			NULL,
			TEXT("IDivide Interface Could Not Be Obtained ... Exiting !!!"),
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
