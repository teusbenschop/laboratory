This is a laboratory setup for building MbedTLS 3 for use in a client on Windows.

Make the modifications described in the Cloud kernel code readme.

Make the modifications that the kernel Windows script makes:
sed -i.bak '/#define MBEDTLS_THREADING_C/d' mbedtls/mbedtls_config.h
sed -i.bak '/#define MBEDTLS_THREADING_PTHREAD/d' mbedtls/mbedtls_config.h

Deal with this error:
LNK2019	unresolved external symbol BCryptGenRandom referenced in function mbedtls_platform_entropy_poll	windows
Uncomment #define MBEDTLS_NO_PLATFORM_ENTROPY
in file mbedtls_config.h

Using the code
* Build the library object files throug $ ./lib.sh
* Build the server through $ ./servers.sh
* Run the server through $ ./servers
* Build the client through $ ./client.sh
* Run the client through $ ./client
