#include <iostream>
#include <string>
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"
#include "mbedtls/build_info.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl_cache.h"
#pragma GCC diagnostic pop


static void throw_mbed_tls_error (const int ret, const char* context)
{
    // Everything OK
    if (!ret)
        return;
    
    // There's an error: Gather info.
    std::string msg {context};
    char error_buf [100];
    mbedtls_strerror (ret, error_buf, 100);
    msg.append (": ");
    msg.append (error_buf);
    msg.append (" (");
    msg.append (std::to_string (ret));
    msg.append (")");
    
    // Throw it.
    throw std::runtime_error (msg);
}


int main()
{
    mbedtls_entropy_context entropy {};
    mbedtls_net_context listen_fd {};
    mbedtls_net_context client_fd {};
    mbedtls_ssl_context ssl {};
    mbedtls_ssl_config conf {};
    mbedtls_ssl_cache_context cache {};
    mbedtls_x509_crt srvcert {};
    mbedtls_pk_context pkey {};
    mbedtls_ctr_drbg_context ctr_drbg {};
    
    try {
        
        constexpr auto debug_level {1};
        mbedtls_debug_set_threshold(debug_level);
        
        std::cout << "Initialize the session data" << std::endl;
        mbedtls_entropy_init(&entropy);
        mbedtls_net_init(&listen_fd);
        mbedtls_net_init(&client_fd);
        mbedtls_ssl_init(&ssl);
        mbedtls_ssl_config_init(&conf);
        mbedtls_ssl_cache_init(&cache);
        mbedtls_x509_crt_init(&srvcert);
        mbedtls_pk_init(&pkey);
        mbedtls_ctr_drbg_init(&ctr_drbg);
        const psa_status_t status = psa_crypto_init();
        throw_mbed_tls_error (status, "Failed to initialize PSA Crypto implementation");
        
        std::cout << "Seeding the random number generator" << std::endl;
        constexpr const auto pers {"server"};
        int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, reinterpret_cast <const unsigned char *>(pers), strlen(pers));
        throw_mbed_tls_error (ret, "Failed to seed the random number generator");
        
        constexpr const auto server_certificate {"/tmp/self-signed-certificates/server/server.crt"};
        std::cout << "Loading the server certificate: " << server_certificate << std::endl;
        ret = mbedtls_x509_crt_parse_file(&srvcert, server_certificate);
        throw_mbed_tls_error (ret, "Failed to load the server certificate");
        constexpr const auto ca_certificate {"/tmp/self-signed-certificates/ca/ca.crt"};
        std::cout << "Loading the CA root certificate: " << ca_certificate << std::endl;
        ret = mbedtls_x509_crt_parse_file(&srvcert, ca_certificate);
        throw_mbed_tls_error (ret, "Failed to load the CA root certificate");
        std::cout << "Loading the server private key" << std::endl;
        constexpr const auto server_key {"/tmp/self-signed-certificates/server/server.key"};
        ret =  mbedtls_pk_parse_keyfile(&pkey, server_key, nullptr, mbedtls_ctr_drbg_random, &ctr_drbg);
        throw_mbed_tls_error (ret, "Failed to load the server private key");
        
        
        constexpr const auto listening_port {"4433"};
        std::cout << "Bind to all interfaces port " << listening_port << std::endl;
        ret = mbedtls_net_bind(&listen_fd, nullptr, listening_port, MBEDTLS_NET_PROTO_TCP);
        //std::cout << "Bind to one interface port " << listening_port << std::endl;
        //ret = mbedtls_net_bind(&listen_fd, "localhost", listening_port, MBEDTLS_NET_PROTO_TCP);
        throw_mbed_tls_error (ret, "Failed to bind");
        
        std::cout << "Setting up the SSL data" << std::endl;
        ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
        throw_mbed_tls_error (ret, "Failed set SSL config defaults");
        mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
        //mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);
        mbedtls_ssl_conf_session_cache(&conf, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
        mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
        ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey);
        throw_mbed_tls_error (ret, "Failed set configure own certificate");
        ret = mbedtls_ssl_setup(&ssl, &conf);
        throw_mbed_tls_error (ret, "Failed SSL setup");
        
        // Keep serving clients.
        while (true) {
            try {
                
                std::cout << "Reset session" << std::endl;
                mbedtls_net_free(&client_fd);
                mbedtls_ssl_session_reset(&ssl);
                
                std::cout << "Waiting for a remote connection" << std::endl;
                ret = mbedtls_net_accept(&listen_fd, &client_fd,  NULL, 0, NULL);
                throw_mbed_tls_error (ret, "Failed accepting connection");
                mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
                
                std::cout << "Performing the SSL/TLS handshake" << std::endl;
                while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
                    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                        throw_mbed_tls_error (ret, "Handshake failure");
                    }
                }
                
                int len {};
                unsigned char buf[1024];
                
                std::cout << "Read from client" << std::endl;
                do {
                    len = sizeof(buf) - 1;
                    memset(buf, 0, sizeof(buf));
                    ret = mbedtls_ssl_read(&ssl, buf, len);
                    
                    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                        continue;
                    }
                    
                    if (ret <= 0) {
                        throw_mbed_tls_error (ret, "Reading error");
                    }
                    
                    len = ret;
                    std::cout << "Bytes read: " << len << std::endl;
                    std::cout << (char *) buf << std::endl;
                    
                    if (len == 0)
                        break;
                    
                    if (ret > 0) {
                        break;
                    }
                } while (true);
                
                std::cout << "Write to client" << std::endl;
                constexpr const auto http_response {"Hello client!"};
                
                len = snprintf((char *) buf, strlen(http_response) + 1, http_response, mbedtls_ssl_get_ciphersuite(&ssl));
                
                while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
                    if (ret == MBEDTLS_ERR_NET_CONN_RESET) {
                        throw std::runtime_error ("Write failed, peer closed the connection");
                    }
                    
                    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                        throw_mbed_tls_error (ret, "Writing error");
                    }
                }
                
                len = ret;
                std::cout << "Bytes written: " << len << ": " << (char *)buf << std::endl;
                
                std::cerr << "Closing the connection" << std::endl;
                
                while ((ret = mbedtls_ssl_close_notify(&ssl)) < 0) {
                    if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                        ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                        throw_mbed_tls_error (ret, "Error closing connection");
                    }
                }
                
            } catch (const std::exception& exception) {
                std::cerr << exception.what() << std::endl;
            }
        }
        
    }
    catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
    }
    
    mbedtls_net_free(&client_fd);
    mbedtls_net_free(&listen_fd);
    mbedtls_x509_crt_free(&srvcert);
    mbedtls_pk_free(&pkey);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_free(&cache);
#endif
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    mbedtls_psa_crypto_free();
#endif /* MBEDTLS_USE_PSA_CRYPTO */
}
