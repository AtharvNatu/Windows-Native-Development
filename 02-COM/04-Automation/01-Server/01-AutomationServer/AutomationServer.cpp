#include <Windows.h>
#include <stdio.h>
#include "AutomationServer.h"

// Class Declarations
class CMyMath : public IMyMath
{
	private:
		long m_cRef;
		ITypeInfo* m_pITypeInfo;

	public:
		// Constructor and Destructor Declarations
		CMyMath(void);
		~CMyMath(void);

		// Inherited IUnknown Method Declarations
		HRESULT __stdcall QueryInterface(REFIID, void**);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);

		// IDispatch(Inherited) Method Declarations
		HRESULT __stdcall GetTypeInfoCount(UINT*);
		HRESULT __stdcall GetTypeInfo(UINT, LCID, ITypeInfo**);
		HRESULT __stdcall GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*);
		HRESULT __stdcall Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);

		// Inherited IMyMath Method Declarations
		HRESULT __stdcall SumOfTwoIntegers(int, int, int*);
		HRESULT __stdcall SubtractionOfTwoIntegers(int, int, int*);

		// Custom Method Declarations
		HRESULT InitTypeLibrary(void);
};

class CMyMathClassFactory :public IClassFactory
{
	private:
		long m_cRef;

	public:
		// Constructor and Destructor Declarations
		CMyMathClassFactory(void);
		~CMyMathClassFactory(void);

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

//  GUID of LIBID_AutomationServer ->{CD57FC48-EDEC-476D-8DE4-A22255E05860}
const GUID LIBID_AutomationServer =
{ 0xcd57fc48, 0xedec, 0x476d, 0x8d, 0xe4, 0xa2, 0x22, 0x55, 0xe0, 0x58, 0x60 };

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

// CMyMath Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CMyMath::CMyMath(void)
{
	// Code
	m_cRef = 1;
	m_pITypeInfo = NULL;
	InterlockedIncrement(&glNumberOfActiveComponents);
}

// Destructor
CMyMath::~CMyMath(void)
{
	// Code
	if (m_pITypeInfo)
	{
		m_pITypeInfo->Release();
		m_pITypeInfo = NULL;
	}

	InterlockedDecrement(&glNumberOfActiveComponents);
}

// Inherited IUnknown Methods
HRESULT CMyMath::QueryInterface(REFIID riid, void** ppv)
{
	// Code
	if (riid == IID_IUnknown)
		*ppv = static_cast<IMyMath*>(this);
	else if (riid == IID_IDispatch)
		*ppv = static_cast<IMyMath*>(this);
	else if (riid == IID_IMyMath)
		*ppv = static_cast<IMyMath*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG CMyMath::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CMyMath::Release(void)
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

// Inherited IDispatch Methods
HRESULT CMyMath::GetTypeInfoCount(UINT * pCountTypeInfo)
{
	// Code
	*pCountTypeInfo = 1;	// Because we have a type library (TLB)
	return S_OK;
}

HRESULT CMyMath::GetTypeInfo(UINT indexOfTypeInfo, LCID lcid, ITypeInfo** ppITypeInfo)
{
	// Code
	*ppITypeInfo = NULL;

	if (indexOfTypeInfo != 0)
		return DISP_E_BADINDEX;

	m_pITypeInfo->AddRef();

	*ppITypeInfo = m_pITypeInfo;

	return S_OK;
}

HRESULT CMyMath::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	// Code
	return DispGetIDsOfNames(
		m_pITypeInfo,
		rgszNames,
		cNames,
		rgDispId
	);
}

HRESULT CMyMath::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	// Code
	HRESULT hr = S_OK;

	hr = DispInvoke(
		this,
		m_pITypeInfo,
		dispIdMember,
		wFlags,
		pDispParams,
		pVarResult,
		pExcepInfo,
		puArgErr
	);

	return hr;
}

// IMyMath's Methods
HRESULT CMyMath::SumOfTwoIntegers(int num1, int num2, int* pSum)
{
	// Code
	*pSum = num1 + num2;
	return S_OK;
}

HRESULT CMyMath::SubtractionOfTwoIntegers(int num1, int num2, int* pSubtract)
{
	// Code
	*pSubtract = num1 - num2;
	return S_OK;
}

HRESULT CMyMath::InitTypeLibrary(void)
{
	// Function Declarations
	void ComErrorDescriptionString(HWND, HRESULT);

	// Variable Declarations
	HRESULT hr;
	ITypeLib* pITypeLib = NULL;

	// Code
	if (m_pITypeInfo == NULL)
	{
		hr = LoadRegTypeLib(
			LIBID_AutomationServer,
			1,	// Major version
			0,	// Minor version
			0x00,
			&pITypeLib
		);

		if (FAILED(hr))
		{
			ComErrorDescriptionString(NULL, hr);
			return hr;
		}

		hr = pITypeLib->GetTypeInfoOfGuid(IID_IMyMath, &m_pITypeInfo);
		if (FAILED(hr))
		{
			ComErrorDescriptionString(NULL, hr);
			pITypeLib->Release();
			pITypeLib = NULL;
			return hr;
		}

		pITypeLib->Release();
		pITypeLib = NULL;
	}

	return S_OK;
}
//-----------------------------------------------------------------------------------------------------

// CMyMathClassFactory Method Implementations
//-----------------------------------------------------------------------------------------------------

// Constructor
CMyMathClassFactory::CMyMathClassFactory(void)
{
	// Code
	m_cRef = 1;
}

// Destructor
CMyMathClassFactory::~CMyMathClassFactory(void)
{
	// Code
}

// Inherited IUnknown Methods
HRESULT CMyMathClassFactory::QueryInterface(REFIID riid, void** ppv)
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

ULONG CMyMathClassFactory::AddRef(void)
{
	// Code
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG CMyMathClassFactory::Release(void)
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
HRESULT CMyMathClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void **ppv)
{
	// Function Declarations
	void ComErrorDescriptionString(HWND, HRESULT);

	// Variable Declarations
	CMyMath* pCMyMath = NULL;
	HRESULT hr = S_OK;

	// Code
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	pCMyMath = new CMyMath();
	if (pCMyMath == NULL)
		return E_OUTOFMEMORY;

	hr = pCMyMath->InitTypeLibrary();
	if (FAILED(hr))
	{
		ComErrorDescriptionString(NULL, hr);
		return hr;
	}

	hr = pCMyMath->QueryInterface(riid, ppv);

	pCMyMath->Release();

	return hr;
}

HRESULT CMyMathClassFactory::LockServer(BOOL fLock)
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
	CMyMathClassFactory* pCSumSubtractClassFactory = NULL;
	HRESULT hr = S_OK;

	// Code
	if (rclsid != CLSID_SumSubtract)
		return CLASS_E_CLASSNOTAVAILABLE;

	pCSumSubtractClassFactory = new CMyMathClassFactory();
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

void ComErrorDescriptionString(HWND hwnd, HRESULT hr)
{
	// Variable Declarationss
	TCHAR* szErrorMsg = NULL;
	TCHAR str[255];

	// Code
	if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
	{
		hr = HRESULT_CODE(hr);

		if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&szErrorMsg,
			0,
			NULL
		) != 0)
		{
			swprintf_s(str, TEXT("%#x : %s"), hr, szErrorMsg);
			LocalFree(szErrorMsg);
			szErrorMsg = NULL;
		}
		else
			swprintf_s(str, TEXT("Unable to find description for error # %#x !!!\n"), hr);

		MessageBox(hwnd, str, TEXT("COM Error"), MB_ICONERROR | MB_OK);
	}
}

//-----------------------------------------------------------------------------------------------------
