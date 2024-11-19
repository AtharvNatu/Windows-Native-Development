import comtypes.client as comclient

CLSID_MyMath = "{69DEFCBC-00E1-4817-A8D1-C608EE5C20F3}"

pIMyMath = comclient.CreateObject(CLSID_MyMath)

num1 = int(input("\nEnter Number 1 = "))
num2 = int(input("\nEnter Number 2 = "))

sum = pIMyMath.SumOfTwoIntegers(num1, num2)
subtraction = pIMyMath.SubtractionOfTwoIntegers(num1, num2)

print("\nSum of {} and {} = {}".format(num1, num2, sum))
print("\nSubtraction of {} and {} = {}\n".format(num1, num2, subtraction))


