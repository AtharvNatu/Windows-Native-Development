using System;
using System.Runtime.InteropServices;
using AutomationServerForDotNet;

public class CSharpAutomation
{
    public static void Main()
    {
        CMyMathClass cMyMathObj = new CMyMathClass();
        IMyMath iMyMathObj = (IMyMath)cMyMathObj;

        int num1 = 30, num2 = 20;
        int result = 0;

        result = iMyMathObj.SumOfTwoIntegers(num1, num2);
        Console.WriteLine("\nSum of " + num1 + " and " + num2 + " = " + result);

        result = iMyMathObj.SubtractionOfTwoIntegers(num1, num2);
        Console.WriteLine("\nSubtraction of " + num1 + " and " + num2 + " = " + result + "\n");
    }
}

