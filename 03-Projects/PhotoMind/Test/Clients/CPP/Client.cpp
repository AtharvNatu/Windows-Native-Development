#include <Windows.h>
#include <iostream>
#include <cstdlib>

CLSID clsid_SdServer;
IDispatch* pIDispatch = NULL;
DISPID dispId_GenerateImage;
OLECHAR* szFunctionName = L"GenerateImage";
VARIANT vArg[2];
VARIANT vRetval;
DISPPARAMS param;

BSTR GenerateImageUsingSD(const wchar_t* promptText, const wchar_t* outputPath)
{
    // Variable Declarations
    CLSID clsid_SdServer;
    IDispatch* pIDispatch = NULL;
    DISPID dispId_GenerateImage;
    LPCOLESTR szFunctionName = L"GenerateImage";
    VARIANT vArg[2];
    VARIANT vRetval;
    DISPPARAMS param;
    BSTR olePrompt = NULL, oleOutputPath = NULL, oleResult = NULL;
    HRESULT hr = S_OK;

    // Code
    hr = CLSIDFromProgID(L"StableDiffusion.Server", &clsid_SdServer);
    if (FAILED(hr))
    {
        std::cerr << "CLSIDFromProgID Failed For StableDiffusion.Server !!!" << std::endl;
        return NULL;
    }
   
    hr = CoCreateInstance(
        clsid_SdServer,
        NULL,
        CLSCTX_LOCAL_SERVER,
        IID_IDispatch,
        (void**)&pIDispatch
    );
    if (FAILED(hr))
    {
        std::cerr << "Failed to obtain Implemented IDispatch Interface of StableDiffusion.Server !!!" << std::endl;
        return NULL;
    }

    VariantInit(&vArg[0]);
    VariantInit(&vArg[1]);
    {
        // const wchar_t* test = L"F:\\Windows-Native-Development\\03-Projects\\Image-Editor\\02-Integration\\Version-03\\PM-Test.png";
        oleResult = NULL;
        olePrompt = SysAllocString(promptText);
        oleOutputPath = SysAllocString(outputPath);

        vArg[0].vt = VT_BSTR;
        vArg[0].bstrVal = oleOutputPath;
        
        vArg[1].vt = VT_BSTR;
        vArg[1].bstrVal = olePrompt;

        param.rgvarg = vArg;
        param.cArgs = 2;
        param.cNamedArgs = 0;
        param.rgdispidNamedArgs = NULL;

        VariantInit(&vRetval);
        {
            // Get DISPID of GenerateImage()
            hr = pIDispatch->GetIDsOfNames(
                IID_NULL,
                (LPOLESTR*)&szFunctionName,
                1,
                GetUserDefaultLCID(),
                &dispId_GenerateImage
            );
            if (FAILED(hr))
            {
                std::cerr << "Failed To Obtain ID For GenerateImage() !!!" << std::endl;
                return NULL;
            }

            hr = pIDispatch->Invoke(
                dispId_GenerateImage,
                IID_NULL,
                GetUserDefaultLCID(),
                DISPATCH_METHOD,
                &param,
                &vRetval,
                NULL,
                NULL
            );
            if (FAILED(hr))
            {
                std::cerr << "Failed To Invoke GenerateImage() !!!" << std::endl;
                return NULL;
            }
            else
            {
                if (vRetval.vt == VT_BSTR)
                    oleResult = SysAllocString(vRetval.bstrVal);
            }
        }
        VariantClear(&vRetval);

        if (oleOutputPath)
        {
            SysFreeString(oleOutputPath);
            oleOutputPath = NULL;
        }

        if (olePrompt)
        {
            SysFreeString(olePrompt);
            olePrompt = NULL;
        }

    }
    VariantClear(&vArg[1]);
    VariantClear(&vArg[0]);

    if (pIDispatch)
	{
		pIDispatch->Release();
		pIDispatch = NULL;
	}

    return oleResult;
}

int main()
{
    // Start COM Engine
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		std::cerr << "Failed to initialize COM Library ... Exiting Now !!!" << std::endl;
		exit(EXIT_FAILURE);
	}

    wchar_t promptText[512] = L"Bunnies in a garden";
    wchar_t outputPath[512] = L"C:\\Users\\Atharv\\Desktop\\Test.png";

    BSTR result = GenerateImageUsingSD(promptText, outputPath);

    int size = WideCharToMultiByte(CP_UTF8, 0, result, -1, NULL, 0, NULL, NULL);
    if (size > 0)
    {
        std::string utf8Result(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, result, -1, &utf8Result[0], size, NULL, NULL);

        // Output to std::cout
        std::cout << "Image Generation Result: " << utf8Result << std::endl;

        SysFreeString(result);
    }

    
    CoUninitialize();

    return 0;
}
