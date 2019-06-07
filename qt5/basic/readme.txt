
$ apt install gcc g++ gdb cmake make build-essential qtcreator qt5-default qtdeclarative5-dev qt5-doc qt5-doc-html qtbase5-doc-html qtbase5-examples

A Qt .pro project must be generated. (This should only be executed once, to generate the file).

$ qmake -project

It will create a file based on the name of the current directory, with the extension .pro. Edit the file to include the following two lines:

QT += core gui
QT += widgets

Now, the application can be compiled and linked into a binary:

qmake
make
./qt5

It should generate a Makefile, make or compile the source code, and start the binary.
If everything works out, a new application window with a small button will appear.

