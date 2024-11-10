#include <stdio.h>
#include <gmp.h>
#include <openssl/sha.h>
#include "generators.h"
#include "algorithms.h"

void init_rsa(mpz_t user_d, mpz_t user_n, mpz_t user_e)
{
    mpz_t number_one, number_two, phi_n, temp, d, e, n;
    mpz_init(number_one);
    mpz_init(number_two);
    mpz_init_set_str(temp, "1", 10);
    mpz_init(n);
    mpz_init(phi_n);
    mpz_init(e);
    mpz_init(d);
    int flag = 0;
    while (flag == 0)
    {
        generate_random_prime(RANDOM_NUMBER_SIZE, number_one);
        // mpz_set_ui(number_one, 3);
        // mpz_set_ui(number_two, 11);
        generate_random_prime(RANDOM_NUMBER_SIZE, number_two);

        mpz_mul(n, number_one, number_two);

        mpz_sub(number_one, number_one, temp);
        mpz_sub(number_two, number_two, temp);
        mpz_mul(phi_n, number_one, number_two);

        mpz_add(number_one, number_one, temp);
        mpz_add(number_two, number_two, temp);
        mpz_init_set_str(e, "7", 10);

        if (mpz_cmp(phi_n, e) <= 0)
        {
            error("Error the phi n is incredibly small");
        }
        extended_enclid(e, phi_n, d);
        if (validate_prime(n, e, d) == 1)
        {
            flag = 1;
        }
    }
    mpz_set(user_e, e);
    mpz_set(user_n, n);
    mpz_set(user_d, d);
}

void calculate_rsa(u_char *message, int message_length,
                   mpz_t n, mpz_t e, unsigned long long int *cipher_message)
{

    // u_char cipher_message[message_length];
    for (int i = 0; i < message_length; i++)
    {
        mpz_t byte;

        mpz_init_set_ui(byte, (unsigned long)message[i]);
        mpz_powm(byte, byte, e, n);

        cipher_message[i] = mpz_get_ui(byte);
        mpz_clear(byte);
    }
}

void decrypt_rsa(unsigned long long int data[], mpz_t n, mpz_t d, u_int data_length, char *decrypted_data)
{
    for (int i = 0; i < data_length; i++)
    {
        mpz_t byte;
        mpz_init_set_ui(byte, data[i]);
        mpz_powm(byte, byte, d, n);

        decrypted_data[i] = (char)mpz_get_ui(byte);
        mpz_clear(byte);
    }
}