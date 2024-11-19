// package main

// import (
// 	ole "github.com/go-ole/go-ole"
// 	"github.com/go-ole/go-ole/oleutil"
// )

// func main() {

// 	// Code
// 	ole.CoInitialize(0)

// 	comObj, _ := oleutil.CreateObject()

// 	pIMyMath := comObj.MustQueryInterface(ole.IID_IDispatch)

// 	print(pIMyMath.GetTypeInfo())

// 	ole.CoUninitialize()
// }

package main

/*
#cgo LDFLAGS: -lOle32
#include <windows.h>
#include <objbase.h>
#include <oleauto.h>

// Function declarations for COM methods we will use.
HRESULT CoInitializeEx(void* pvReserved, DWORD dwCoInit);
void CoUninitialize();
HRESULT CoCreateInstance(const CLSID *rclsid, void* pvReserved, DWORD dwClsCtx, const IID *riid, void** ppv);
HRESULT IDispatch_Invoke(IDispatch *pdisp, DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *rgvarg, VARIANT *rgvarRes, EXCEPINFO *rgexcepinfo, UINT *rguiArgErr);
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// Define GUIDs, CLSID, and IID for the COM object and interface
// These would need to be replaced with the actual GUIDs for the COM object you are working with
var CLSID_SomeCOMObject = C.GUID{
	Data1: 0x12345678,
	Data2: 0x1234,
	Data3: 0x1234,
	Data4: [8]C.Uint8{0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF},
}

var IID_IDispatch = C.GUID{
	Data1: 0x00020400,
	Data2: 0x0000,
	Data3: 0x0000,
	Data4: [8]C.Uint8{0xC0, 0x00, 0x0000, 0x000000000046},
}

func main() {
	// Initialize COM library for the current thread
	if hr := C.CoInitializeEx(nil, C.COINIT_APARTMENTTHREADED); hr != 0 {
		fmt.Printf("Failed to initialize COM: HRESULT = %x\\n", hr)
		return
	}
	defer C.CoUninitialize()

	// Create COM object using CLSID_SomeCOMObject
	var ppv unsafe.Pointer
	hr := C.CoCreateInstance(&CLSID_SomeCOMObject, nil, C.CLSCTX_ALL, &IID_IDispatch, &ppv)
	if hr != 0 {
		fmt.Printf("Failed to create COM object: HRESULT = %x\\n", hr)
		return
	}

	// Cast ppv to IDispatch interface (You may need to implement this part depending on your COM object)
	idispatch := (*C.IDispatch)(ppv)

	// Use IDispatch to invoke a method (replace with actual method and DISPID)
	// Here, we are invoking a sample method (you must know the DISPID and parameter structure)
	dispid := C.DISPID(1) // The DISPID for the method (this is an example)
	var params C.DISPPARAMS
	var result C.VARIANT
	var excepInfo C.EXCEPINFO
	var argErr C.UINT

	hr = C.IDispatch_Invoke(idispatch, dispid, nil, 0, C.DISPATCH_METHOD, &params, &result, &excepInfo, &argErr)
	if hr != 0 {
		fmt.Printf("Failed to invoke method: HRESULT = %x\\n", hr)
		return
	}

	// Process the result (you may need to map the result to Go types)
	// Here we simply print the result (this will depend on the method you are calling)
	fmt.Printf("Method invoked successfully, result: %v\\n", result)

	// If needed, remember to release the IDispatch interface
	// (For example, using `IUnknown_Release` if necessary)
}
