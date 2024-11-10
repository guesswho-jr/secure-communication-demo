#include <stdio.h>
#include <math.h>
#include <gmp.h>

int extended_enclid(mpz_t e, mpz_t phi, mpz_t d)
{
    // mpz_invert(d, e, phi);
    mpz_t divisor, remainder, dividend, quotient, x0, x1, potential_d;
    mpz_init_set(divisor, e);
    mpz_init_set_ui(remainder, -1);
    mpz_init_set(dividend, phi);
    mpz_init(quotient);
    mpz_init_set_str(x0, "0", 10);
    mpz_init_set_str(x1, "1", 10);
    mpz_init_set_str(potential_d, "0", 10);
    while (mpz_cmp_ui(remainder, 0) > 0)
    {
        mpz_div(quotient, dividend, divisor);
        mpz_mod(remainder, dividend, divisor);
        mpz_set(dividend, divisor);
        mpz_set(divisor, remainder);
        if (mpz_cmp_ui(remainder, 0) > 0)
        {
            mpz_t temp, temp1;
            mpz_init(temp);
            mpz_init(temp1);
            mpz_set(temp, x0);
            mpz_set(x0, x1);
            mpz_mul(temp1, quotient, x1);
            mpz_sub(x1, temp, temp1);
            mpz_clear(temp);
            mpz_clear(temp1);
        }
    }
    if (mpz_cmp_ui(x1, 0) < 0)
    {
        mpz_add(x1, x1, phi);
    }

    mpz_set(d, x1);
    mpz_clear(remainder);
    mpz_clear(quotient);
    mpz_clear(x1);
    mpz_clear(x0);
    mpz_init(potential_d);
}
