import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:win32/win32.dart';

const clsIdMyMath = '{69DEFCBC-00E1-4817-A8D1-C608EE5C20F3}';

(int, int) calculateResult(int num1, int num2) {
  //* Code
  int sum = 0, subtraction = 0;

  //! Initialize COM
  CoInitializeEx(
    nullptr,
    COINIT.COINIT_APARTMENTTHREADED | COINIT.COINIT_DISABLE_OLE1DDE,
  );

  Dispatcher dispatcher = Dispatcher.fromCLSID(clsIdMyMath);

  final dispParams = calloc<DISPPARAMS>();
  final vArg = calloc<VARIANT>(2);

  VariantInit(vArg);
  {
    vArg[0].vt = VARENUM.VT_INT;
    vArg[0].intVal = num2;

    vArg[1].vt = VARENUM.VT_INT;
    vArg[1].intVal = num1;

    dispParams.ref.cArgs = 2;
    dispParams.ref.rgvarg = vArg;

    final vRetVal = calloc<VARIANT>();
    VariantInit(vRetVal);
    {
      dispatcher.invoke("SumOfTwoIntegers", dispParams, vRetVal);
      sum = vRetVal.ref.intVal;

      dispatcher.invoke("SubtractionOfTwoIntegers", dispParams, vRetVal);
      subtraction = vRetVal.ref.intVal;
    }
    VariantClear(vRetVal);
    free(vRetVal);
  }
  VariantClear(vArg);

  free(vArg);
  free(dispParams);

  dispatcher.dispose();

  //! Uninitialize COM
  CoUninitialize();

  return (sum, subtraction);
}
