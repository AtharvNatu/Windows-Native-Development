package com.automation.server;

public class JavaCOMServer
{
    private native boolean __initializeCOM();
    private native void __uninitializeCOM();
    private native boolean __createInstance(String clsID);
    private native int __invoke(String functionName, int num1, int num2);

    public boolean CoInitialize()
    {
        return __initializeCOM();
    }

    public boolean CoCreateInstance(String clsID)
    {
        return __createInstance(clsID);
    }

    public int Invoke(String functionName, int num1, int num2)
    {
        return __invoke(functionName, num1, num2);
    }

    public void CoUninitialize()
    {
        __uninitializeCOM();
    }
}

