import "package:flutter/material.dart";
import "package:flutter_automation_client/flutter_automation.dart";

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: "Flutter COM Automation Client",
      theme: ThemeData(
        useMaterial3: true,
      ),
      home: const MyHomePage(title: "Flutter COM Automation Client"),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  //* Variables
  final TextEditingController num1Controller = TextEditingController();
  final TextEditingController num2Controller = TextEditingController();

  //* Code
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightBlue,
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            SizedBox(
              width: 300,
              child: TextField(
                controller: num1Controller,
                style: const TextStyle(color: Colors.black),
                expands: false,
                keyboardType: TextInputType.number,
                decoration: const InputDecoration(
                  hintText: "Enter Number 1",
                ),
              ),
            ),
            const SizedBox(height: 40),
            SizedBox(
              width: 300,
              child: TextField(
                controller: num2Controller,
                expands: false,
                style: const TextStyle(color: Colors.black),
                keyboardType: TextInputType.number,
                decoration: const InputDecoration(
                  hintText: "Enter Number 2",
                ),
              ),
            ),
            const SizedBox(height: 60),
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(
                backgroundColor: Colors.lightBlue,
                foregroundColor: Colors.white,
                minimumSize: const Size(200, 50),
              ),
              label: const Text("Calculate Result"),
              onPressed: () {
                // Call COM Functions using IDispatch
                int num1 = int.parse(num1Controller.text);
                int num2 = int.parse(num2Controller.text);

                var (addition, subtraction) = calculateResult(num1, num2);

                // Show Addition Result
                showDialog<String>(
                  context: context,
                  builder: (BuildContext context) => AlertDialog(
                    title: const Text("Addition Result"),
                    content: Text("Addition of $num1 and $num2 = $addition"),
                    actions: <Widget>[
                      TextButton(
                        onPressed: () => {
                          Navigator.pop(context, "Ok"),
                          // Show Subtraction Result
                          showDialog<String>(
                            context: context,
                            builder: (BuildContext context) => AlertDialog(
                              title: const Text("Subtraction Result"),
                              content: Text(
                                  "Subtraction of $num1 and $num2 = $subtraction"),
                              actions: <Widget>[
                                TextButton(
                                  onPressed: () => Navigator.pop(context, "Ok"),
                                  child: const Text("Ok"),
                                ),
                              ],
                            ),
                          ),
                        },
                        child: const Text("Ok"),
                      ),
                    ],
                  ),
                );
              },
            ),
          ],
        ),
      ),
    );
  }

  @override
  void dispose() {
    num2Controller.dispose();
    num1Controller.dispose();
    super.dispose();
  }
}
