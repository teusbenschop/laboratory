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
#include <thread>
#include <string.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>

using namespace std;


mbedtls_entropy_context be_mbed_tls_entropy;
mbedtls_ctr_drbg_context be_mbed_tls_ctr_drbg;
mbedtls_x509_crt be_mbed_tls_cacert;


void display_mbed_tls_error (int & ret, string * error)
{
  // Local copy of the return value, and clear the original return value.
  int local_return = ret;
  ret = 0;
  
  // Everything OK
  if (local_return == 0) return;
  
  // There's an error: Display it.
  char error_buf [100];
  mbedtls_strerror (local_return, error_buf, 100);
  string msg = error_buf;
  msg.append (" (");
  msg.append (to_string (local_return));
  msg.append (")");
  if (error) {
    error->assign (msg);
  } else {
    cerr << msg << endl;
  }
}


void filter_url_ssl_tls_initialize ()
{
  int ret = 0;
  // Random number generator.
  mbedtls_ctr_drbg_init (&be_mbed_tls_ctr_drbg);
  mbedtls_entropy_init (&be_mbed_tls_entropy);
  const char *pers = "Client";
  ret = mbedtls_ctr_drbg_seed (&be_mbed_tls_ctr_drbg, mbedtls_entropy_func, &be_mbed_tls_entropy, (const unsigned char *) pers, strlen (pers));
  display_mbed_tls_error (ret, NULL);
  // Wait until the trusted root certificates exist.
  // This is necessary as there's cases that the data is still being installed at this point.
  string path = "cas.crt";
  //while (!file_or_dir_exists (path)) this_thread::sleep_for (chrono::milliseconds (100));
  // Read the trusted root certificates.
  ret = mbedtls_x509_crt_parse_file (&be_mbed_tls_cacert, path.c_str ());
  display_mbed_tls_error (ret, NULL);
}


int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  // Initialize the SSL/TLS system once.
  mbedtls_ctr_drbg_init (&be_mbed_tls_ctr_drbg);
  mbedtls_entropy_init (&be_mbed_tls_entropy);
  const char *pers = "Bibledit Client";
  mbedtls_ctr_drbg_seed (&be_mbed_tls_ctr_drbg,
                         mbedtls_entropy_func,
                         &be_mbed_tls_entropy,
                         (const unsigned char *) pers,
                         strlen (pers));

  int ret = 0;
  string path = "cas.crt";
  ret = mbedtls_x509_crt_parse_file (&be_mbed_tls_cacert, path.c_str ());
  display_mbed_tls_error (ret, NULL);

  
  // Initialize SSL/TLS (after webroot has been set).
  new thread (filter_url_ssl_tls_initialize);

  
  this_thread::sleep_for (chrono::seconds (1));

  // Finalize the SSL/TLS system once.
  mbedtls_ctr_drbg_free (&be_mbed_tls_ctr_drbg);
  mbedtls_entropy_free (&be_mbed_tls_entropy);
  mbedtls_x509_crt_free (&be_mbed_tls_cacert);
  
  return EXIT_SUCCESS;
}
