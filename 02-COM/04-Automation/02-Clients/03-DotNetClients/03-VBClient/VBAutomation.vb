Imports System.Windows.Forms
Imports AutomationServerForDotNet

Public Class VBAutomation
    Inherits Form

    Public Sub New()

        Dim MyIDispatch As Object
        Dim MyRef As New CMyMathClass

        MyIDispatch = MyRef

        Dim iNum1 = 30
        Dim iNum2 = 20

        Dim iSum = MyIDispatch.SumOfTwoIntegers(iNum1, iNum2)
        Dim str As String = String.Format("Sum of {0} and {1} is {2}", iNum1, iNum2, iSum)
        MsgBox(str)

        Dim iSubtract = MyIDispatch.SubtractionOfTwoIntegers(iNum1, iNum2)
        str = String.Format("Subtraction of {0} and {1} is {2}", iNum1, iNum2, iSubtract)
        MsgBox(str)

        End

    End Sub

    <STAThread>
    Shared Sub Main()
        Application.EnableVisualStyles()
        Application.Run(New VBAutomation())
    End Sub

End Class