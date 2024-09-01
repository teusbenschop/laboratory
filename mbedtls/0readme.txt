This is a laboratory setup for building MbedTLS 3 for use in a client on Windows.

Make the modifications described in the Cloud kernel code readme.

Make the modifications that the kernel Windows script makes:
sed -i.bak '/#define MBEDTLS_THREADING_C/d' mbedtls/mbedtls_config.h
sed -i.bak '/#define MBEDTLS_THREADING_PTHREAD/d' mbedtls/mbedtls_config.h

Deal with this error:
LNK2019	unresolved external symbol BCryptGenRandom referenced in function mbedtls_platform_entropy_poll	windows
Add the following to the client.cpp code:
#pragma comment(lib, "bcrypt.lib")

Using the code
* Build the library object files on Linux throug $ ./lib.sh
* Build the server on Linux through $ ./servers.sh
* Run the server on Linux through $ ./servers
* Build the client on Linux through $ ./client.sh
* Run the client on Linux through $ ./client
Or
* Build the clienton Visual Studio on Windows.
* Run the client on Windows through $ ./client.exe

