#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <openssl/sha.h>

#include <string.h>
typedef unsigned long long int random_number_type;
typedef unsigned int ptr;
void error(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}

void *error_checked_malloc(unsigned int alloc_size)
{
    void *allocated = malloc(alloc_size);
    if (allocated == NULL)
    {
        error("Unable to allocate memory!!");
    }
    return allocated;
}

void *error_checked_realloc(size_t alloc_size, u_char *buffer, size_t *initial_size)
{
    *initial_size += alloc_size;
    void *allocated = realloc(buffer, *initial_size);
    if (allocated == NULL)
    {
        error("Unable to allocate memory!!");
    }
    return allocated;
}
void *error_checked_realloc_byte(size_t alloc_size, unsigned long long int *buffer, size_t *initial_size)
{
    *initial_size += alloc_size;
    void *allocated = realloc(buffer, *initial_size);
    if (allocated == NULL)
    {
        error("Unable to allocate memory!!");
    }
    return allocated;
}
int validate_prime(mpz_t n, mpz_t e, mpz_t d)

{
    mpz_t msg, c, ans;
    mpz_init(c);
    mpz_init(ans);
    mpz_init_set_str(msg, "123", 10);
    mpz_powm(c, msg, e, n);
    mpz_powm(ans, c, d, n);
    if (mpz_cmp(msg, ans) == 0)
    {
        mpz_clear(msg);
        mpz_clear(ans);
        mpz_clear(c);
        return 1;
    }
    else
    {
        mpz_clear(msg);
        mpz_clear(ans);
        mpz_clear(c);
        return 0;
    }
}

void sha256(const char *data, unsigned char *hash, int data_length)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, data_length);
    SHA256_Final(hash, &ctx);
}

char *merge(char *string_one, char *string_two, int string_one_length, int string_two_length)
{
    char *merged = (char *)error_checked_malloc(string_one_length + string_two_length);
    for (size_t i = 0; i < string_one_length; i++)
    {
        merged[i] = string_one[i];
    }

    for (size_t i = 0; i < string_two_length; i++)
    {
        merged[i + string_one_length - 1] = string_two[i];
        // printf("%c", string_two[i]);
        // printf("%c", merged[string_one_length - 1]);
    }
    merged[string_one_length + string_two_length] = 0;
    return merged;
}

void split_and_set(char *string, char split, mpz_t n, mpz_t e, u_int total_length)
{
    int pos = 0;
    for (size_t i = 0; i < total_length; i++)
    {
        if (string[i] == split)
        {
            pos = i;
            break;
        }
    }

    char *part_one = (char *)error_checked_malloc(pos + 1);
    u_int alloc_size = total_length - (pos + 2);
    char part_two[alloc_size];
    for (size_t i = 0; i < pos; i++)
    {
        part_one[i] = string[i];
    }
    part_one[pos] = 0;
    pos += 3;
    for (size_t i = 0; i < alloc_size; i++)
    {

        part_two[i] = string[pos + i];
    }
    part_two[alloc_size] = 0;
    mpz_set_str(n, part_one, 10);
    mpz_set_str(e, part_two, 10);

    free(part_one);
}
// Alternative

char *input(char *message, size_t *output_length)
{
    printf("%s", message);
    // puts("I");

    char *response = (char *)error_checked_malloc(1);
    char byte;
    int i = 0;
    while (1)
    {
        scanf("%c", &byte);
        if (byte == '\n')
        {
            break;
        }
        response = (char *)error_checked_realloc(1, response, output_length);
        response[i] = byte;
        i++;
    }
    return response;
}
