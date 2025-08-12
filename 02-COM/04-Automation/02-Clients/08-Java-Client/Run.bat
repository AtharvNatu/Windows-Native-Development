cls

javac com\\automation\\server\\JavaCOMServer.java
javac com\\automation\\client\\Client.java

java --enable-native-access=ALL-UNNAMED -Djava.library.path=com\\automation\\client -cp . com.automation.client.Client
