Cobol Hello World Example
https://www.thegeekstuff.com/2010/02/cobol-hello-world-example-how-to-write-compile-and-execute-cobol-program-on-linux-os/
Remarks:
Compile as $ cobc -x -o helloworld helloworld.cob
Strict source formatting, as visible in the .cob source.

Calculator in Cobol
See calculator.cob
Compile:
$ cobc -free -x -o calculator calculator.cob
Run:
$ ./calculator
Output:
COBOL CALCULATOR
Enter First Number : 10
Enter Operator (+,-,*,/): +
Enter Second Number: 22
10  + 22  =       32.00
end of program, please press a key to exit

