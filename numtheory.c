#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "randstate.h"
#include <gmp.h>

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t v, p, D;
    mpz_inits(v, p, D, NULL);
    mpz_set(D, d);

    mpz_set_ui(v, 1);
    mpz_set(p, a);
    while (mpz_cmp_ui(D, 0) > 0) {
        if (mpz_odd_p(D)) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_fdiv_q_ui(D, D, 2);
    }
    mpz_set(o, v);
    mpz_clears(v, p, D, NULL);
}

bool is_prime(mpz_t n, uint64_t iters) {
    mpz_t a, V, n_prime, one, s, two, r, jah, n_prime2, y, j, s_prime;
    mpz_inits(a, V, n_prime, s, one, two, r, jah, n_prime2, y, j, s_prime, NULL);

    //add cases
    mpz_set_ui(two, 2); // two
    mpz_set_ui(one, 1); // one
    mpz_sub_ui(n_prime, n, 1); // nprime = n - 1
    mpz_set_ui(s, 0); //set s as zero

    uint64_t s_2 = 0;

    mpz_mul_2exp(jah, one, s_2); //jah = 2^s 2^s

    mpz_fdiv_q(r, n_prime, jah); //r = n-1/ 2^s

    mpz_sub_ui(n_prime2, n, 3); // n-2

    if (mpz_cmp_ui(n, 2) == 0) {
        return true;
    }

    if (mpz_cmp_ui(n, 1) == 0) {
        return false;
    }

    if (mpz_cmp_ui(n, 0) == 0) {
        return false;
    }
    if (mpz_cmp_ui(n, 3) == 0) {
        return true;
    }

    while (mpz_even_p(r)) { // if r is even, s increases
        mpz_add_ui(s, s, 1); //adds 1 to s
        mpz_fdiv_q_ui(r, r, 2); //power of s increases r = r/2
        s_2++;
    }

    for (uint32_t i = 1; i <= iters; i++) {
        mpz_urandomm(a, state, n_prime2);
        mpz_add_ui(a, a, 2);
        mpz_set(V, r);
        pow_mod(y, a, V, n);
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_prime) != 0)) {
            mpz_set_ui(j, 1);
            mpz_sub_ui(s_prime, s, 1);
            while (mpz_cmp(j, s_prime) <= 0 && mpz_cmp(y, n_prime) != 0) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, n_prime) != 0) {
                return false;
            }
        }
    }
    return true;
    mpz_clears(a, V, n_prime, s, one, two, r, jah, n_prime2, y, j, s_prime, NULL);
}

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t t, A, B;
    mpz_inits(t, A, B, NULL);

    mpz_set(A, a);
    mpz_set(B, b);

    while (mpz_cmp_si(B, 0) != 0) {
        mpz_set(t, B);
        mpz_mod(B, A, B);
        mpz_set(A, t);
    }
    mpz_set(g, A);
    mpz_clears(t, A, B, NULL);
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r, r_prime, t, t_prime, q, temp_r, tt;
    mpz_inits(r, r_prime, t, t_prime, q, temp_r, tt, NULL);

    mpz_set(r, n);
    mpz_set(r_prime, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);

    while (mpz_cmp_ui(r_prime, 0) != 0) {
        mpz_fdiv_q(q, r, r_prime); // q<- r/r'

        mpz_set(temp_r, r); //stores value of r
        mpz_set(r, r_prime); // r <- r' prime

        mpz_mul(r_prime, q, r_prime); // q <- q x r'

        mpz_sub(r_prime, temp_r, r_prime); // r' <- r - q x r'

        mpz_set(tt, t); //tt <- temp t
        mpz_set(t, t_prime); //t <- t'
        mpz_mul(t_prime, q, t_prime); // q <- q x t'

        mpz_sub(t_prime, tt, t_prime); //t' <- tt - q
    }
    if (mpz_cmp_si(r, 1) > 0) {
        mpz_set_ui(o, 0);
        return;
    }
    if (mpz_cmp_si(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(o, t);
    mpz_clears(r, r_prime, t, t_prime, q, temp_r, tt, NULL);
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t P;
    mpz_init(P);

    mpz_set(P, p);
    do {
        mpz_urandomb(P, state, bits);

    } while (!is_prime(P, iters));
    mpz_set(p, P);
}
