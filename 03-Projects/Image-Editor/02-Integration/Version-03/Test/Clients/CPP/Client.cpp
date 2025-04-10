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

int main()
{
    // Start COM Engine
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		std::cerr << "Failed to initialize COM Library ... Exiting Now !!!" << std::endl;
		exit(EXIT_FAILURE);
	}

    hr = CLSIDFromProgID(L"StableDiffusion.Server", &clsid_SdServer);
    if (FAILED(hr))
    {
        std::cerr << "CLSIDFromProgID Failed !!!" << std::endl;
		exit(EXIT_FAILURE);
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
        std::cerr << "Failed to obtain Implemented IDispatch Interface !!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    VariantInit(vArg);
    {
        vArg[0].vt = VT_BSTR;
        vArg[0].bstrVal = SysAllocString(L"F:\\Windows-Native-Development\\03-Projects\\Image-Editor\\02-Integration\\Version-03\\Test\\Clients\\CPP\\Output.png");
        
        vArg[1].vt = VT_BSTR;
        vArg[1].bstrVal = SysAllocString(L"Ducks on vacation");

        param.rgvarg = vArg;
        param.cArgs = 2;
        param.cNamedArgs = 0;
        param.rgdispidNamedArgs = NULL;

        VariantInit(&vRetval);
        {
            // Get DISPID of GenerateImage()
            hr = pIDispatch->GetIDsOfNames(
                IID_NULL,
                &szFunctionName,
                1,
                GetUserDefaultLCID(),
                &dispId_GenerateImage
            );
            if (FAILED(hr))
            {
                std::cerr << "Failed To Obtain ID For GenerateImage() !!!" << std::endl;
                exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
            }
            else
            {
                if (vRetval.vt == VT_BSTR)
                    std::wcout << L"Server Result : " << vRetval.bstrVal << std::endl;
            }
        }
        VariantClear(&vRetval);
    }
    VariantClear(vArg);

    if (pIDispatch)
	{
		pIDispatch->Release();
		pIDispatch = NULL;
	}

    CoUninitialize();

    return 0;
}
