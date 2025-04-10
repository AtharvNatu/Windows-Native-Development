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

// #include <windows.h>
// #include <comdef.h>
// #include <iostream>

// int main() {
//     HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
//     if (FAILED(hr)) {
//         std::cerr << "Failed to initialize COM.\n";
//         return 1;
//     }

//     CLSID clsid;
//     hr = CLSIDFromProgID(OLESTR("StableDiffusion.Server"), &clsid);
//     if (FAILED(hr)) {
//         std::cerr << "CLSIDFromProgID failed.\n";
//         CoUninitialize();
//         return 1;
//     }

//     IDispatch* pDisp = nullptr;
//     hr = CoCreateInstance(clsid, nullptr, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&pDisp);
//     if (FAILED(hr)) {
//         std::cerr << "CoCreateInstance failed.\n";
//         CoUninitialize();
//         return 1;
//     }

//     // Prepare arguments: outputPath and prompt
//     _bstr_t prompt(L"a fantasy castle in the clouds, 8K resolution");
//     _bstr_t outputPath(L"F:\\Windows-Native-Development\\03-Projects\\Image-Editor\\02-Integration\\Version-03\\Test\\Clients\\CPP\\Output.png");

//     VARIANTARG args[2];
//     VariantInit(&args[0]);
//     VariantInit(&args[1]);

//     args[1].vt = VT_BSTR;
//     args[1].bstrVal = prompt;

//     args[0].vt = VT_BSTR;
//     args[0].bstrVal = outputPath;

//     DISPPARAMS params = { args, nullptr, 2, 0 };
//     VARIANT result;
//     VariantInit(&result);

//     // Get DISPID of GenerateImage
//     OLECHAR* methodName = L"GenerateImage";
//     DISPID dispid;
//     hr = pDisp->GetIDsOfNames(IID_NULL, &methodName, 1, LOCALE_USER_DEFAULT, &dispid);
//     if (FAILED(hr)) {
//         std::cerr << "GetIDsOfNames failed.\n";
//         pDisp->Release();
//         CoUninitialize();
//         return 1;
//     }

//     // Invoke method
//     hr = pDisp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &result, nullptr, nullptr);
//     if (FAILED(hr)) {
//         std::cerr << "Invoke failed. HRESULT = 0x" << std::hex << hr << std::endl;
//     } else {
//         if (result.vt == VT_BSTR) {
//             std::wcout << L"Server Result: " << result.bstrVal << std::endl;
//         } else {
//             std::cout << "Method returned unexpected type.\n";
//         }
//     }

//     // Cleanup
//     VariantClear(&args[0]);
//     VariantClear(&args[1]);
//     VariantClear(&result);
//     pDisp->Release();
//     CoUninitialize();
//     return 0;
// }
