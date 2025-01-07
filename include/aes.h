#include <openssl/evp.h>
#include <openssl/err.h>

int aes_encrypt(u_char *plaintext, int plaintext_length, u_char *key, u_char *iv, u_char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        error("Error while decrypting");
    }
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        error("Error occured while decrypting");
    }
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_length) != 1)
    {
        error("Error occured while decrypting");
    }
    ciphertext_len = len;
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
    {
        error("Error occured while decrypting");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int aes_decrypt(u_char *ciphertext, int ciphertext_len, u_char *iv, u_char *plaintext, u_char *key)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        ERR_print_errors_fp(stderr);
        error("Error decrypting");
        exit(1);
    }
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        ERR_print_errors_fp(stderr);
        error("Error occured while decrypting (init)");
    }
    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1)
    {
        ERR_print_errors_fp(stderr);
        error("Error occured while decrypting(update)");
    }
    plaintext_len = len;
    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1)
    {
        ERR_print_errors_fp(stderr);
        puts("Invalid characters received. Please check what you sent.");
        return -1;
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}
