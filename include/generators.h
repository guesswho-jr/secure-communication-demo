#include <stdlib.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <gmp.h>
#include <time.h>
#include "constants.h"
#include "utils.h"

long long int generate_random_seed()
{

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {
        error("Failed to open file!!");
    }
    char seed[SEED_SIZE] = {0};

    read(fd, &seed, SEED_SIZE);

    for (int i = 0; i < sizeof(seed); i++)
    {
        if ((int)seed[i] < 0)
        {
            seed[i] = -1 * seed[i];
        }
    }
    long long int seed_value = time(0);
    for (int i = 0; i < sizeof(seed); i++)
    {
        seed_value = seed_value << 8 | seed[i];
    }
    close(fd);
    return seed_value;
}

char generate_random_byte()
{
    long long int seed = generate_random_seed();
    srand(seed);
    char byte = 0x00;
    char digit_one = (char)(rand() % (15) + 1);
    char digit_two = (char)(rand() % (15) + 1);
    byte = digit_one << 4 | digit_two;
    return byte;
}

void generate_random_number(int size, mpz_t bignum)
{

    u_char *numbers = (u_char *)error_checked_malloc(size);
    for (int i = 0; i < size; i++)
    {
        u_char byte = generate_random_byte();
        numbers[i] = numbers[i] << 8 | byte;
    }

    mpz_import(bignum, RANDOM_NUMBER_SIZE, 1, 1, 1, 0, numbers);

    free(numbers);
}

u_char *generate_random_array(int size)
{
    u_char *numbers = (u_char *)error_checked_malloc(size);
    for (int i = 0; i < size; i++)
    {
        u_char byte = generate_random_byte();
        numbers[i] = numbers[i] << 8 | byte;
    }
    numbers[size] = 0;
    return numbers;
}
int search(int array[], int what, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (array[i] == what)
        {
            return i;
        }
    }
    return 0;
}
int remove_from_array(int array[], int what, int size)
{
    int index = search(array, what, size);
    array[index] = 0;
}

void generate_random_prime(int size, mpz_t bignum)
{
    generate_random_number(size, bignum);
    mpz_setbit(bignum, 0);
    mpz_nextprime(bignum, bignum);
    int flag = 0;
    while (flag == 0)
    {
        generate_random_number(size, bignum);
        mpz_setbit(bignum, 0);
        mpz_nextprime(bignum, bignum);

        if (mpz_probab_prime_p(bignum, 50) > 0)
        {
            flag = 1;
        }
    }
}
