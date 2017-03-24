#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include <string.h>
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"


using namespace std;

// SSL/TLS globals.
mbedtls_entropy_context filter_url_mbed_tls_entropy;
mbedtls_ctr_drbg_context filter_url_mbed_tls_ctr_drbg;
mbedtls_x509_crt filter_url_mbed_tls_cacert;


int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
    

  mbedtls_ctr_drbg_init (&filter_url_mbed_tls_ctr_drbg);
  mbedtls_entropy_init (&filter_url_mbed_tls_entropy);
  const char *pers = "Bibledit Client";
  mbedtls_ctr_drbg_seed (&filter_url_mbed_tls_ctr_drbg,
                               mbedtls_entropy_func, &filter_url_mbed_tls_entropy,
                               (const unsigned char *) pers, strlen (pers));

  string path = "cas.crt";
  mbedtls_x509_crt_parse_file (&filter_url_mbed_tls_cacert, path.c_str ());


  return EXIT_SUCCESS;
}
