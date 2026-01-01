
#include <algorithm>
#include <array>
#include <iostream>
#include <iomanip>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Shared/AesWrapper.h>


const std::string hmac_passphrase {"HMACpass123!@#"};


void handle_errors()
{
    ERR_print_errors_fp(stderr);
    std::abort();
}


// C-code AES Encrypter.
int encrypt(EVP_CIPHER_CTX *ctx, EVP_CIPHER* cipher,
            unsigned char *plaintext, int plaintext_len,
            unsigned char *key, unsigned char *iv,
            unsigned char *ciphertext)
{
    // Reset context before any reuse.
    EVP_CIPHER_CTX_reset(ctx);

    if(!EVP_EncryptInit_ex2(ctx, cipher, key, iv, nullptr))
        handle_errors();

    int len{};
    int ciphertext_len{};

    if(!EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handle_errors();
    ciphertext_len = len;
    
    if(!EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handle_errors();
    ciphertext_len += len;
    
    std::cout << "Ciphertext length: " << ciphertext_len << std::endl;
    return ciphertext_len;
}


// C-code AES Decrypter.
int decrypt(EVP_CIPHER_CTX *ctx, EVP_CIPHER* cipher,
            unsigned char *ciphertext, int ciphertext_len,
            unsigned char *key, unsigned char *iv,
            unsigned char *plaintext)
{
    // Reset context before any reuse.
    EVP_CIPHER_CTX_reset(ctx);
    
    if(!EVP_DecryptInit_ex(ctx, cipher, nullptr, key, iv))
        handle_errors();

    int len{};
    int plaintext_len{};

    if(!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handle_errors();
    plaintext_len = len;
    
    if(!EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handle_errors();
    plaintext_len += len;
    
    return plaintext_len;
}


// The C-code style in this block is less than optimal, don't use it in modern C++.
TEST(AesWrapper, c_code)
{
    // Use one cipher context for all operations.
    EVP_CIPHER_CTX *ctx;
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handle_errors();
    
    // Get the cipher to be used, one for all operations.
    EVP_CIPHER* cipher {EVP_CIPHER_fetch(nullptr, "AES-256-CBC", nullptr)};

    const std::string long_input = "HMACpass123!@# The quick brown fox jumps over the lazy dog";
    for (size_t i {1}; i <= long_input.size(); i++) {
        const std::string secret (long_input.substr(0, i));
        std::cout << "Encrypting " << std::quoted(secret) << " length " << secret.size() << std::endl;

        // Random generator.
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution random_byte(0, 255);

        // Create the salt for the key.
        std::array<unsigned char, 8> salt {};
        std::ranges::generate(salt, [&]() { return random_byte(gen); });

        // Generate the 256 bit encryption key.
        std::array<unsigned char, 32> key  {};
        if (!PKCS5_PBKDF2_HMAC(hmac_passphrase.data(), static_cast<int>(hmac_passphrase.size()),
                               salt.data(), static_cast<int>(salt.size()),
                               10'000,
                               EVP_sha3_256(),
                               static_cast<int>(key.size()),
                               key.data()))
            handle_errors();

        // Generate a new 128 bit Initialisation Vector (IV) for every encryption / decryption.
        std::array<unsigned char, 16> iv {};
        std::ranges::generate(iv, [&]() { return random_byte(gen); });
        
        // Message to be encrypted.
        unsigned char *plaintext = (unsigned char *)(secret.c_str());
        
        // Buffer for ciphertext.
        // Ensure the buffer is long enough for the ciphertext
        // which may be longer than the plaintext
        // depending on the algorithm and mode.
        unsigned char ciphertext[128];
        
        // Buffer for the decrypted text.
        unsigned char decryptedtext[128];
        
        int decryptedtext_len{}, ciphertext_len{};
        
        // Encrypt the plaintext.
        ciphertext_len = encrypt (ctx, cipher, plaintext, strlen ((char *)plaintext), key.data(), iv.data(), ciphertext);
        
        // Print the ciphertext for diagnostics.
        //std::cout << "Ciphertext is:" << std::endl;
        //BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);
        
        // Decrypt the ciphertext again.
        decryptedtext_len = decrypt(ctx, cipher, ciphertext, ciphertext_len, key.data(), iv.data(), decryptedtext);
        
        // Show the decrypted text.
        std::string printable_decrypted_text ((const char*)decryptedtext, decryptedtext_len);
        //std::cout << "Decrypted: " << std::quoted(printable_decrypted_text) << std::endl;

        EXPECT_EQ (printable_decrypted_text, secret);
    }

    // Free the cipher and its context.
    EVP_CIPHER_CTX_free(ctx);
    EVP_CIPHER_free(cipher);
}
