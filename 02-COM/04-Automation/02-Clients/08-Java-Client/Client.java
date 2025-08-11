public class Client 
{
    public static void main(String[] args) 
    {
        final String clsID = "{69DEFCBC-00E1-4817-A8D1-C608EE5C20F3}";

        System.loadLibrary("JavaCOMWrapper");

        JavaCOMWrapper jcw = new JavaCOMWrapper();

        boolean ret = jcw.CoInitialize();
        if (!ret)
        {
            System.err.println("Failed To Initialize COM Library ... Exiting !!!");
            System.exit(-1);
        }

        ret = jcw.CoCreateInstance(clsID);
        if (!ret)
        {
            System.err.println("Failed To Obtain Implemented IDispatch Interface ... Exiting !!!");
            System.exit(-1);
        }

        int result = jcw.Invoke("SumOfTwoIntegers", 10, 654);
        System.out.println("Addition = " + result);

        result = jcw.Invoke("SubtractionOfTwoIntegers", 30, 20);
        System.out.println("Subtraction = " + result);

        jcw.CoUninitialize();
    }
}
