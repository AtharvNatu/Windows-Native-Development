#include "com_automation_server_JavaCOMServer.h"
#include "AutomationServer.h"

IDispatch* pIDispatch = NULL;
HRESULT hr = NULL;

JNIEXPORT jboolean JNICALL Java_com_automation_server_JavaCOMServer__1_1initializeCOM(JNIEnv *, jobject)
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

JNIEXPORT jboolean JNICALL Java_com_automation_server_JavaCOMServer__1_1createInstance(JNIEnv *env, jobject, jstring clsID)
{
    IID iidMyMath;
    OLECHAR* szClsId = ConvertJStringToOleChar(env, clsID);

    hr = IIDFromString(szClsId, &iidMyMath);

    hr = CoCreateInstance(
				iidMyMath,
				NULL,
				CLSCTX_INPROC_SERVER,
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


JNIEXPORT jint JNICALL Java_com_automation_server_JavaCOMServer__1_1invoke(JNIEnv* env, jobject, jstring functionName, jint iNum1, jint iNum2)
{
    DISPID dispId;
    DISPPARAMS param;

    VARIANT vArg[2];
    VARIANT vRetval;

    OLECHAR* szFunctionName = ConvertJStringToOleChar(env, functionName);

    VariantInit(vArg);
    {
        vArg[0].vt = VT_INT;
        vArg[0].intVal = iNum2;
        
        vArg[1].vt = VT_INT;
        vArg[1].intVal = iNum1;

        param.rgvarg = vArg;
        param.cArgs = 2;
        param.cNamedArgs = 0;
        param.rgdispidNamedArgs = NULL;

        VariantInit(&vRetval);
        {
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

    if (szFunctionName)
        delete[] szFunctionName;
}

JNIEXPORT void JNICALL Java_com_automation_server_JavaCOMServer__1_1uninitializeCOM(JNIEnv *, jobject)
{
    if (pIDispatch)
	{
		pIDispatch->Release();
		pIDispatch = NULL;
	}
    
    CoUninitialize();
}

