import 'dart:io';
import 'package:dart_automation_client/dart_automation.dart';

void main() {
  // Code
  stdout.write("\nEnter Number 1 = ");
  int? num1 = int.parse(stdin.readLineSync()!);

  stdout.write("\nEnter Number 2 = ");
  int? num2 = int.parse(stdin.readLineSync()!);

  var (sum, subtraction) = calculateResult(num1, num2);

  print("\nAddition of $num1 and $num2 = $sum");
  print("\nSubtraction of $num1 and $num2 = $subtraction");
}
