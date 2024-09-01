#!/bin/bash


mbed_tls_files=(
  mbedtls/aes.c
  mbedtls/aesce.c
  mbedtls/aesni.c
  mbedtls/aria.c
  mbedtls/asn1parse.c
  mbedtls/asn1write.c
  mbedtls/base64.c
  mbedtls/bignum.c
  mbedtls/bignum_core.c
  mbedtls/bignum_mod.c
  mbedtls/bignum_mod_raw.c
  mbedtls/block_cipher.c
  mbedtls/camellia.c
  mbedtls/ccm.c
  mbedtls/chacha20.c
  mbedtls/chachapoly.c
  mbedtls/cipher.c
  mbedtls/cipher_wrap.c
  mbedtls/cmac.c
  mbedtls/constant_time.c
  mbedtls/ctr_drbg.c
  mbedtls/debug.c
  mbedtls/des.c
  mbedtls/dhm.c
  mbedtls/ecdh.c
  mbedtls/ecdsa.c
  mbedtls/ecjpake.c
  mbedtls/ecp.c
  mbedtls/ecp_curves.c
  mbedtls/ecp_curves_new.c
  mbedtls/entropy.c
  mbedtls/entropy_poll.c
  mbedtls/error.c
  mbedtls/gcm.c
  mbedtls/hkdf.c
  mbedtls/hmac_drbg.c
  mbedtls/lmots.c
  mbedtls/lms.c
  mbedtls/md.c
  mbedtls/md5.c
  mbedtls/memory_buffer_alloc.c
  mbedtls/mps_reader.c
  mbedtls/mps_trace.c
  mbedtls/net_sockets.c
  mbedtls/nist_kw.c
  mbedtls/oid.c
  mbedtls/padlock.c
  mbedtls/pem.c
  mbedtls/pk.c
  mbedtls/pk_ecc.c
  mbedtls/pk_wrap.c
  mbedtls/pkcs12.c
  mbedtls/pkcs5.c
  mbedtls/pkcs7.c
  mbedtls/pkparse.c
  mbedtls/pkwrite.c
  mbedtls/platform.c
  mbedtls/platform_util.c
  mbedtls/poly1305.c
  mbedtls/psa_crypto.c
  mbedtls/psa_crypto_aead.c
  mbedtls/psa_crypto_cipher.c
  mbedtls/psa_crypto_client.c
  mbedtls/psa_crypto_driver_wrappers_no_static.c
  mbedtls/psa_crypto_ecp.c
  mbedtls/psa_crypto_ffdh.c
  mbedtls/psa_crypto_hash.c
  mbedtls/psa_crypto_mac.c
  mbedtls/psa_crypto_pake.c
  mbedtls/psa_crypto_rsa.c
  mbedtls/psa_crypto_se.c
  mbedtls/psa_crypto_slot_management.c
  mbedtls/psa_crypto_storage.c
  mbedtls/psa_its_file.c
  mbedtls/psa_util.c
  mbedtls/ripemd160.c
  mbedtls/rsa.c
  mbedtls/rsa_alt_helpers.c
  mbedtls/sha1.c
  mbedtls/sha256.c
  mbedtls/sha3.c
  mbedtls/sha512.c
  mbedtls/ssl_cache.c
  mbedtls/ssl_ciphersuites.c
  mbedtls/ssl_client.c
  mbedtls/ssl_cookie.c
  mbedtls/ssl_debug_helpers_generated.c
  mbedtls/ssl_msg.c
  mbedtls/ssl_ticket.c
  mbedtls/ssl_tls.c
  mbedtls/ssl_tls12_client.c
  mbedtls/ssl_tls12_server.c
  mbedtls/ssl_tls13_client.c
  mbedtls/ssl_tls13_generic.c
  mbedtls/ssl_tls13_keys.c
  mbedtls/ssl_tls13_server.c
  mbedtls/threading.c
  mbedtls/timing.c
  mbedtls/version.c
  mbedtls/version_features.c
  mbedtls/x509.c
  mbedtls/x509_create.c
  mbedtls/x509_crl.c
  mbedtls/x509_crt.c
  mbedtls/x509_csr.c
  mbedtls/x509write.c
  mbedtls/x509write_crt.c
  mbedtls/x509write_csr.c
)

find . -name "*.o" -delete
if [ $? -ne 0 ]; then exit 1; fi

#for input_file in ${mbed_tls_files[@]}; do
for input_file in mbedtls/*.c; do
  output_file=${input_file%.c}.o
  echo Compile $input_file '->' $output_file
  gcc -I. -c -o $output_file.o $input_file
done

rm -rf mbedtls/.deps
if [ $? -ne 0 ]; then exit 1; fi
rm -rf mbedtls/.dirstamp
if [ $? -ne 0 ]; then exit 1; fi

find . -name main -delete
if [ $? -ne 0 ]; then exit 1; fi

echo Build main binary
g++ -Wall -Wextra -pedantic -std=c++17 -lpthread \
-o main \
main.cpp \
mbedtls/*.o \
-I. \
#-I/opt/local/include -L/opt/local/lib \
#-lmbedtls -lmbedcrypto -lmbedx509 \

if [ $? -ne 0 ]; then exit 1; fi

