#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include <thread>
#include <string.h>
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"
#include <mbedtls/build_info.h>
#include <mbedtls/platform.h>
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#pragma GCC diagnostic pop


// Static check on required definitions, taken from the ssl_client1.c example.
#ifndef MBEDTLS_BIGNUM_C
static_assert (false, "MBEDTLS_BIGNUM_C should be defined");
#endif
#ifndef MBEDTLS_ENTROPY_C
static_assert (false, "MBEDTLS_ENTROPY_C should be defined");
#endif
#ifndef MBEDTLS_SSL_TLS_C
static_assert (false, "MBEDTLS_SSL_TLS_C should be defined");
#endif
#ifndef MBEDTLS_SSL_CLI_C
static_assert (false, "MBEDTLS_SSL_CLI_C should be defined");
#endif
#ifndef MBEDTLS_NET_C
static_assert (false, "MBEDTLS_NET_C should be defined");
#endif
#ifndef MBEDTLS_RSA_C
static_assert (false, "MBEDTLS_RSA_C should be defined");
#endif
#ifndef MBEDTLS_PEM_PARSE_C
static_assert (false, "MBEDTLS_PEM_PARSE_C should be defined");
#endif
#ifndef MBEDTLS_CTR_DRBG_C
static_assert (false, "MBEDTLS_CTR_DRBG_C should be defined");
#endif
#ifndef MBEDTLS_X509_CRT_PARSE_C
static_assert (false, "MBEDTLS_X509_CRT_PARSE_C should be defined");
#endif
#ifndef MBEDTLS_DEBUG_C
static_assert (false, "MBEDTLS_DEBUG_C should be defined");
#endif
//#ifndef MBEDTLS_USE_PSA_CRYPTO
//static_assert (false, "MBEDTLS_USE_PSA_CRYPTO should be defined");
//#endif
#ifdef MBEDTLS_X509_REMOVE_INFO
static_assert (false, "MBEDTLS_X509_REMOVE_INFO should not be defined");
#endif


static void my_debug(void *ctx, [[maybe_unused]]int level,
                     const char *file, int line,
                     const char *str)
{
    mbedtls_fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *) ctx);
}


static void display_mbed_tls_error (const int ret, const char* context, std::string* error)
{
    // Everything OK
    if (!ret)
        return;
    
    // There's an error: Display it.
    std::string msg {context};
    char error_buf [100];
    mbedtls_strerror (ret, error_buf, 100);
    msg.append (": ");
    msg.append (error_buf);
    msg.append (" (");
    msg.append (std::to_string (ret));
    msg.append (")");
    if (error) {
        error->assign (msg);
    } else {
        std::cerr << msg << std::endl;
    }
}


int main([[maybe_unused]]int argc, [[maybe_unused]]char *argv[])
{
    constexpr auto debug_level {0};
    mbedtls_debug_set_threshold(debug_level);

    std::cout << "Initialize the session data" << std::endl;
    mbedtls_net_context server_fd {};
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_context ssl {};
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config conf {};
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt cacert {};
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_context ctr_drbg {};
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_context entropy {};
    mbedtls_entropy_init(&entropy);
    const psa_status_t status = psa_crypto_init();
    display_mbed_tls_error (status, "Failed to initialize PSA Crypto implementation", nullptr);

    std::cout << "Seeding the random number generator" << std::endl;
    constexpr const auto pers {"client"};
    int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, reinterpret_cast <const unsigned char *>(pers), strlen(pers));
    display_mbed_tls_error (ret, "Failed to seed the random number generator", nullptr);

    std::cout << "Loading the CA root certificate" << std::endl;
    constexpr const auto ca_certificate {"/tmp/self-signed-certificates/ca/ca.crt"};
    ret = mbedtls_x509_crt_parse_file(&cacert, ca_certificate);
    display_mbed_tls_error (ret, "Failed to load the CA root certificate", nullptr);

    constexpr auto server_name {"localhost"};
    constexpr auto server_port {"4433"};
    std::cout << "Connecting to tcp " << server_name << ":" << server_port << std::endl;
    ret = mbedtls_net_connect(&server_fd, server_name, server_port, MBEDTLS_NET_PROTO_TCP);
    display_mbed_tls_error (ret, "Failed to connect to the server", nullptr);
    
    std::cout << "Setting up the SSL/TLS structure" << std::endl;
    ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    display_mbed_tls_error (ret, "Failed to set SSL/TLS defaults", nullptr);
    // Value OPTIONAL is not optimal for security, but makes things easier just now.
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
//    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);
    ret = mbedtls_ssl_setup(&ssl, &conf);
    display_mbed_tls_error (ret, "Failed to setup SSL", nullptr);
    ret = mbedtls_ssl_set_hostname(&ssl, server_name);
    display_mbed_tls_error (ret, "Failed to set the host name", nullptr);
    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    std::cout << "Performing the SSL/TLS handshake" << std::endl;
    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            display_mbed_tls_error (ret, "Handshake failure", nullptr);
        }
    }

    std::cout << "Verifying peer X.509 certificate" << std::endl;
    uint32_t flags = mbedtls_ssl_get_verify_result(&ssl);
    if (flags != 0) {
        char vrfy_buf[512];
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "", flags);
        std::cerr << "Failed to verify peer certificate: " << vrfy_buf << std::endl;
    }

    constexpr auto get_request {"Hello server!"};
    std::cout << "Write to server: " << get_request << std::endl;
    unsigned char buf[1024] {};
    int len = snprintf(reinterpret_cast <char*>(buf), strlen(get_request)+1, "%s", get_request);
    while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            display_mbed_tls_error (ret, "Failed to write to server", nullptr);
        }
    }
    len = ret;
    std::cout << "Bytes written: " << len << std::endl;

    std::cout << "Read from server:" << std::endl;
    do {
        len = sizeof(buf) - 1;
        memset(buf, 0, sizeof(buf));
        ret = mbedtls_ssl_read(&ssl, buf, len);
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            break;
        }
        if (ret < 0) {
            display_mbed_tls_error (ret, "Failed to read from server", nullptr);
            break;
        }
        if (ret == 0) {
            std::cout << "\n\nEOF\n\n" << std::endl;
            break;
        }
        len = ret;
        std::cout << "Bytes read: " << len << std::endl;
        std::cout << (char *) buf << std::endl;
    } while (true);

    std::cout << "Close connection" << std::endl;
    mbedtls_ssl_close_notify(&ssl);

    std::cout << "Free SSL structures" << std::endl;
    mbedtls_net_free(&server_fd);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_psa_crypto_free();
  
    return EXIT_SUCCESS;
}
