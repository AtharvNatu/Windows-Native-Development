#include "JavaCOMWrapper.h"
#include "AutomationServer.h"

IDispatch* pIDispatch = NULL;
HRESULT hr = NULL;

JNIEXPORT jboolean JNICALL Java_JavaCOMWrapper__1_1initializeCOM(JNIEnv *, jobject)
{
    hr = CoInitialize(NULL);
    if (FAILED(hr))
        return false;

    return true;
}

OLECHAR* ConvertJStringToOleChar(JNIEnv* env, jstring jStr)
{
    if (!jStr)
        return nullptr;

    jsize length = env->GetStringLength(jStr);

    const jchar* raw = env->GetStringChars(jStr, nullptr);

    OLECHAR* buffer = new WCHAR[length + 1];
    memcpy(buffer, raw, length * sizeof(OLECHAR));
    buffer[length] = L'\0';

    env->ReleaseStringChars(jStr, raw);

    return buffer;
}

JNIEXPORT jboolean JNICALL Java_JavaCOMWrapper__1_1createInstance(JNIEnv *env, jobject, jstring clsID)
{
    IID iidMyMath;
    OLECHAR* szClsId = ConvertJStringToOleChar(env, clsID);

    hr = IIDFromString(szClsId, &iidMyMath);

    hr = CoCreateInstance(
				iidMyMath,
				NULL,						// NULL specifies No Aggregation
				CLSCTX_INPROC_SERVER,		// CLSCTX_INPROC_SERVER => Server Type is DLL
				IID_IDispatch,
				(void**)&pIDispatch
			);

    if (FAILED(hr))
    {
        if (szClsId)
            delete[] szClsId;
        return false;
    }

    if (szClsId)
        delete[] szClsId;

    return true;
}


JNIEXPORT jint JNICALL Java_JavaCOMWrapper__1_1invoke(JNIEnv* env, jobject, jstring functionName, jintArray args)
{
    DISPID dispId;
    DISPPARAMS param;

    VARIANT vArg[_ARRAYSIZE(args)];
    VARIANT vRetval;

    OLECHAR* szFunctionName = ConvertJStringToOleChar(env, functionName);

    int argCount = env->GetArrayLength(args);

    jint* numbers = env->GetIntArrayElements(args, nullptr);

    VariantInit(vArg);
    {
        for (int i = argCount; i >= 0; i--)
        {
            vArg[i].vt = VT_INT;
            vArg[i].intVal = numbers[i];
        }

        param.rgvarg = vArg;
        param.cArgs = argCount;
        param.cNamedArgs = 0;
        param.rgdispidNamedArgs = NULL;

        VariantInit(&vRetval);
        {
            // Get DISPID of SumOfTwoIntegers()
            hr = pIDispatch->GetIDsOfNames(
                IID_NULL,
                &szFunctionName,
                1,
                GetUserDefaultLCID(),
                &dispId
            );
            if (FAILED(hr))
                return -1;

            hr = pIDispatch->Invoke(
                dispId,
                IID_NULL,
                GetUserDefaultLCID(),
                DISPATCH_METHOD,
                &param,
                &vRetval,
                NULL,
                NULL
            );
            if (FAILED(hr))
                return -1;
            else
                return vRetval.lVal;
        }
        VariantClear(&vRetval);
    }
    VariantClear(vArg);

    env->ReleaseIntArrayElements(args, numbers, 0);
    numbers = nullptr;

    if (szFunctionName)
        delete[] szFunctionName;
}

JNIEXPORT void JNICALL Java_JavaCOMWrapper__1_1uninitializeCOM(JNIEnv *, jobject)
{
    if (pIDispatch)
	{
		pIDispatch->Release();
		pIDispatch = NULL;
	}
    
    CoUninitialize();
}

