#define PORT 1234
#define EOL "\r\n"
#define EOL_SIZE 2
#define INITIAL_BUFFER_SIZE 2
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <netinet/in.h>
#include "aes.h"

int cmp(char one[], char two[], int one_size, int two_size)
{
    if (one_size != two_size)
    {
        return 0;
    }
    for (size_t i = 0; i < one_size; i++)
    {
        if (one[i] != two[i])
        {
            return 0;
        }
    }
    return 1;
}

u_char *receive_string(int sockfd, size_t *initial_size, char stop)
{

    u_char *dest_buffer = (u_char *)error_checked_malloc((u_int)*initial_size);
    u_char byte;
    int i = 0;
    size_t total_length = 0, recv_length = 0;
    while (1)
    {
        u_char byte;
        recv_length = recv(sockfd, &byte, 1, 0);
        // printf("%ld", recv_length);
        if (recv_length <= 0 || byte == stop)
        {
            // printf("Got %02x\n", byte);
            return dest_buffer;
        }
        if (total_length >= *initial_size)
        {
            dest_buffer = (u_char *)error_checked_realloc(1, dest_buffer, initial_size);
        }
        dest_buffer[i] = byte;
        total_length++;
        i++;
    }
}

void copy_string(char *buffer, char *string, int length)
{
    for (size_t i = 0; i < length; i++)
    {
        memset(buffer, string[i], 1);
    }
}
void send_encrypted_string(char *message, mpz_t n, mpz_t e, int sockfd, int message_length)
{

    u_char sha_checksum_raw[SHA256_DIGEST_LENGTH];
    sha256(message, sha_checksum_raw, message_length);

    u_char *KEY = generate_random_array(AES_KEY_SIZE);
    u_char *IV = generate_random_array(AES_IV_SIZE);
    int length = round((message_length / 16) + 1) * 16;
    u_char aes_ciphertext[length + 1];
    for (size_t i = 0; i <= length; i++)
    {
        aes_ciphertext[i] = 0;
    }

    int cipher_len = aes_encrypt(message, message_length, KEY, IV, aes_ciphertext);
    // u_char real_cipher_text[cipher_len];

    // for (size_t i = 0; i < cipher_len; i++)
    // {
    //     printf("%02x ", aes_ciphertext[i]);
    // }
    // puts("");
    u_char end_aes = '\0';
    strcat(aes_ciphertext, &end_aes);
    unsigned long long int sha_checksum_enc[SHA256_DIGEST_LENGTH];
    unsigned long long int key_enc[AES_KEY_SIZE];
    unsigned long long int iv_enc[AES_IV_SIZE];

    calculate_rsa(sha_checksum_raw, SHA256_DIGEST_LENGTH, n, e, sha_checksum_enc);
    calculate_rsa(KEY, AES_KEY_SIZE, n, e, key_enc);
    calculate_rsa(IV, AES_IV_SIZE, n, e, iv_enc);
    char sep = '-';
    char end[] = "E";

    u_char *buffer = (u_char *)error_checked_malloc(message_length + 1);

    send(sockfd, sha_checksum_enc, 256, 0);
    send(sockfd, key_enc, AES_KEY_SIZE * 8, 0);
    send(sockfd, iv_enc, AES_IV_SIZE * 8, 0);
    send(sockfd, aes_ciphertext, cipher_len + 1, 0);
    free(buffer);
    free(KEY);
    free(IV);
}

int get_encrypted_string(mpz_t d, mpz_t n, int sockfd, size_t *message_len, char *ip)
{
    size_t initial_size = 8;
    int recv_length;
    size_t total_received;
    unsigned long long int *sha_checksum_enc = (unsigned long long int *)error_checked_malloc(initial_size * 32);
    recv_length = recv(sockfd, sha_checksum_enc, initial_size * 32, 0);
    if (recv_length < initial_size * 32)
    {
        puts("Cannot get data (connection might be closed)");
        return 0;
    }
    unsigned long long int *KEY_enc = (unsigned long long int *)error_checked_malloc(AES_KEY_SIZE * initial_size);
    u_char sha_checksum[32];

    recv_length = recv(sockfd, KEY_enc, AES_KEY_SIZE * initial_size, 0);
    if (recv_length < initial_size * AES_KEY_SIZE)
    {
        puts("Cannot get data (connection might be closed)");
        return 0;
    }

    unsigned long long int *IV_enc = (unsigned long long int *)error_checked_malloc(AES_IV_SIZE * initial_size);
    recv_length = recv(sockfd, IV_enc, AES_IV_SIZE * initial_size, 0);
    if (recv_length < initial_size * AES_IV_SIZE)
    {
        puts("Cannot get data (connection might be closed) ");
        return 0;
    }
    u_char KEY[AES_KEY_SIZE];
    u_char IV[AES_IV_SIZE];

    size_t total_mem = 1;
    u_char *aes_ciphertext = receive_string(sockfd, &total_mem, '\0');

    // for (size_t i = 0; i < total_mem; i++)
    // {
    //     printf("%02x ", aes_ciphertext[i]);
    // }
    // puts("");
    char decrypted_text[total_mem];
    printf("\nDecrypting the RSA credentials...");
    decrypt_rsa(sha_checksum_enc, n, d, 32, sha_checksum);
    decrypt_rsa(KEY_enc, n, d, AES_KEY_SIZE, KEY);
    decrypt_rsa(IV_enc, n, d, AES_IV_SIZE, IV);
    printf("done\n");
    printf("\nDecrypting the message... ");
    int plain_text_len = aes_decrypt(aes_ciphertext, total_mem, IV, decrypted_text, KEY);

    printf("done\n");
    u_char sha_validation[32];
    sha256(decrypted_text, sha_validation, plain_text_len);
    printf("Validating message after decryption... ");
    if (cmp(sha_checksum, sha_validation, 32, 32) == 1)
    {
        printf("done\n");
        printf("Received: ");
        for (size_t i = 0; i < plain_text_len; i++)
        {
            printf("%c", decrypted_text[i]);
        }
        printf(" from %s", ip);
    }
    else
    {
        puts("Cannot validate the checksum. Message might have been altered.");
    }
    puts("");

    free(sha_checksum_enc);
    free(KEY_enc);
    free(IV_enc);
    free(aes_ciphertext);
    return 1;
}