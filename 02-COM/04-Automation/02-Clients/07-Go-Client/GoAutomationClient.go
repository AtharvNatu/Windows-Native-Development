package main

import (
	"fmt"

	ole "github.com/go-ole/go-ole"
)

func main() {

	// Code
	ole.CoInitialize(0)
	defer ole.CoUninitialize()

	CLSID_MyMath := "{69DEFCBC-00E1-4817-A8D1-C608EE5C20F3}"

	clsid_ptr, err := ole.CLSIDFromString(CLSID_MyMath)
	if err != nil {
		fmt.Println("Error Occurred When Retrieving Co-Class With Provided CLSID !!!")
		return
	}

	iUnknown_ptr, err := ole.CreateInstance(clsid_ptr, ole.IID_IUnknown)
	if err != nil {
		fmt.Println("Failed To Obtain Implemented IID_IUnknown Interface !!!")
		return
	}
	defer iUnknown_ptr.Release()

	dispatch_ptr, err := iUnknown_ptr.QueryInterface(ole.IID_IDispatch)
	if err != nil {
		fmt.Println("Failed To Obtain Implemented IID_IDispatch Interface !!!")
		return
	}
	defer dispatch_ptr.Release()

	var dispid []int32
	dispid, err = dispatch_ptr.GetIDsOfName([]string{"SumOfTwoIntegers"})
	if err != nil {
		fmt.Println("Failed To Obtain ID For SumOfTwoIntegers() !!!")
		return
	}

	var result *ole.VARIANT
	result, err = dispatch_ptr.Invoke(dispid[0], ole.DISPATCH_METHOD, 2, 5)
	if err != nil {
		fmt.Println("Failed To Obtain Result For SumOfTwoIntegers() !!!")
		return
	}

	sum := result.Value()
	fmt.Println("Sum = ", sum)

}
