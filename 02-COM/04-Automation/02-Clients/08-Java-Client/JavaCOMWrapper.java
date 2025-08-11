public class JavaCOMWrapper 
{
    private native boolean __initializeCOM();
    private native void __uninitializeCOM();
    private native boolean __createInstance(String clsID);
    private native int __invoke(String functionName, int... numbers);

    public boolean CoInitialize()
    {
        return __initializeCOM();
    }

    public boolean CoCreateInstance(String clsID)
    {
        return __createInstance(clsID);
    }

    public int Invoke(String functionName, int... numbers)
    {
        return __invoke(functionName, numbers);
    }

    public void CoUninitialize()
    {
        __uninitializeCOM();
    }
}

