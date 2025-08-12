package com.automation.client;

import com.automation.server.JavaCOMServer;
import java.util.Scanner;

public class Client 
{
    public static void main(String[] args) 
    {
        final String clsID = "{69DEFCBC-00E1-4817-A8D1-C608EE5C20F3}";

        System.loadLibrary("JavaCOMServer");

        JavaCOMServer server = new JavaCOMServer();

        boolean ret = server.CoInitialize();
        if (!ret)
        {
            System.err.println("Failed To Initialize COM Library ... Exiting !!!");
            System.exit(-1);
        }

        ret = server.CoCreateInstance(clsID);
        if (!ret)
        {
            System.err.println("Failed To Obtain Implemented IDispatch Interface ... Exiting !!!");
            System.exit(-1);
        }

        Scanner scanner = new Scanner(System.in);

        System.out.print("\nEnter Number 1 = ");
        int num1 = scanner.nextInt();
        System.out.print("\nEnter Number 2 = ");
        int num2 = scanner.nextInt();

        int result = server.Invoke("SumOfTwoIntegers", num1, num2);
        System.out.println("\nAddition = " + result);

        result = server.Invoke("SubtractionOfTwoIntegers", num1, num2);
        System.out.println("Subtraction = " + result);

        scanner.close();
        server.CoUninitialize();
    }
}
