#include "ImageToolkit.h"

// Interface Declaration
interface INoAggregationIUnknown
{
	virtual HRESULT __stdcall QueryInterface_NoAggregation(REFIID, void**) = 0;
	virtual ULONG __stdcall AddRef_NoAggregation(void) = 0;
	virtual ULONG __stdcall Release_NoAggregation(void) = 0;
};

// Class Declarations
class CImageToolkit :public INoAggregationIUnknown, IDesaturation, ISepia, IColorInversion
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

		// Inherited IColorInversion Method Declarations
		HRESULT __stdcall ColorInversion(COLORREF, COLORREF*);
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
HRESULT CImageToolkit::Desaturation(cv::Mat& image)
{
	// Code
	for (size_t i = 0; i < image.rows; i++)
	{
		for (size_t j = 0; j < image.cols; j++)
		{
			cv::Vec3b& rgbVector = image.at<cv::Vec3b>(i, j);
			uchar grayscaleValue = static_cast<uchar>(
				(0.3f * rgbVector[2]) +
				(0.59f * rgbVector[1]) +
				(0.11f * rgbVector[0])
			);

			rgbVector[0] = grayscaleValue;
			rgbVector[1] = grayscaleValue;
			rgbVector[2] = grayscaleValue;
		}
	}

	return S_OK;
}

// ISepia's Methods
HRESULT CImageToolkit::Sepia(cv::Mat& image)
{
	// Code
	for (size_t i = 0; i < image.rows; i++)
	{
		for (size_t j = 0; j < image.cols; j++)
		{
			cv::Vec3b& rgbVector = image.at<cv::Vec3b>(i, j);

			float sepiaR = ((0.393f * static_cast<float>(rgbVector[2])) + (0.769f * static_cast<float>(rgbVector[1])) + (0.189f * static_cast<float>(rgbVector[0])));
			float sepiaG = ((0.349f * static_cast<float>(rgbVector[2])) + (0.686f * static_cast<float>(rgbVector[1])) + (0.168f * static_cast<float>(rgbVector[0])));
			float sepiaB = ((0.272f * static_cast<float>(rgbVector[2])) + (0.534f * static_cast<float>(rgbVector[1])) + (0.131f * static_cast<float>(rgbVector[0])));

			rgbVector[0] = static_cast<uchar>(std::min(255.0f, sepiaB));
			rgbVector[1] = static_cast<uchar>(std::min(255.0f, sepiaG));
			rgbVector[2] = static_cast<uchar>(std::min(255.0f, sepiaR));
		}
	}

	return S_OK;
}

// IColorInversion's Methods
HRESULT CImageToolkit::ColorInversion(cv::Mat& image)
{
	// Code
	for (size_t i = 0; i < image.rows; i++)
	{
		for (size_t j = 0; j < image.cols; j++)
		{
			cv::Vec3b& rgbVector = image.at<cv::Vec3b>(i, j);

			rgbVector[0] = 255 - rgbVector[0];
			rgbVector[1] = 255 - rgbVector[1];
			rgbVector[2] = 255 - rgbVector[2];
		}
	}

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

extern "C" HRESULT __stdcall DllRegisterServer(void)
{
	// Variable Declarations
	HKEY hCLSIDKey = NULL, hInProcServerKey = NULL;
	LONG lRet;
	TCHAR szModulePath[MAX_PATH];
	TCHAR szClassDescription[] = TEXT("Image Toolkit Component");
	TCHAR szThreadingModel[] = TEXT("Apartment");

	// Code
	__try
	{
		lRet = RegCreateKeyEx(
			HKEY_CLASSES_ROOT, 
			TEXT("CLSID\\{2F5B06BE-BF26-4AC1-B64F-5DE34974419D}"), 
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
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\{2F5B06BE-BF26-4AC1-B64F-5DE34974419D}\\InProcServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\{2F5B06BE-BF26-4AC1-B64F-5DE34974419D}"));

	return S_OK;
}
//-----------------------------------------------------------------------------------------------------
