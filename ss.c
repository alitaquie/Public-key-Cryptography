#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "numtheory.h"
#include "randstate.h"
#include <gmp.h>

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    //generate pbits !!! :D
    mpz_t P, Q, qprime, pprime;
    mpz_inits(P, Q, qprime, pprime, NULL);
    uint64_t lower_bound = nbits / 5; //sets bounds for pbits lower range
    uint64_t upper_bound = (2 * nbits) / 5; //sets bounds for pbits range
    uint64_t pbits
        = (random() % (upper_bound - lower_bound)) + lower_bound; //generates pbits using the range
    //now lets find the qbits
    uint64_t qbits = nbits - (2 * pbits);
    do {
        make_prime(p, pbits + 1, iters);
        make_prime(q, qbits + 2, iters);
        mpz_sub_ui(pprime, p, 1);
        mpz_sub_ui(qprime, q, 1);
        mpz_mod(P, qprime, p); //q-1 MOD p
        mpz_mod(Q, pprime, q); //p-1 MOD

        mpz_mul(n, p, p);
        mpz_mul(n, n, q);

        uint64_t final_bits = mpz_sizeinbase(n, 2);
        if (final_bits < nbits) {
            continue;
        }
    } while ((mpz_cmp_si(P, 0) == 0) && (mpz_cmp_si(Q, 0) == 0)); // if its divisible and mod == 0

    mpz_clears(P, Q, qprime, pprime, NULL);
}

void ss_make_priv(mpz_t d, mpz_t pq, mpz_t p, mpz_t q) {
    mpz_t gc, lcm, n, pp, qq;
    mpz_inits(gc, lcm, n, pp, qq, NULL);
    mpz_mul(n, p, p);
    mpz_mul(n, n, q); // n complete
    mpz_sub_ui(pp, p, 1); //p-1
    mpz_sub_ui(qq, q, 1); //q-1
    mpz_mul(pq, pp, qq); // initializes pq = p-1 * q-1
    gcd(gc, pp, qq); // potential error..? gc = gcd(p-1,q-1)
    //p^2 using this for n p * p
    mpz_fdiv_q(lcm, pq, gc); // lcm == pq/gcd(p,q); p-1 * q-1 / gcd(p-1,q-1)
    //mpz_mul(n,p_2,q); // n = p^2 * q
    mod_inverse(d, n, lcm);
    mpz_mul(pq, p, q);
    mpz_clears(gc, lcm, n, pp, qq, NULL);
    //lcm hacks (a * b)/gcd(a,b)
}

void ss_write_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    fprintf(pbfile, "%s\n", username);
}

void ss_write_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d);
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    fscanf(pbfile, "%s\n", username);
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", pq, d);
}

void ss_encrypt(mpz_t c, mpz_t m, mpz_t n) {
    pow_mod(c, m, n, n);
}

void ss_encrypt_file(FILE *infile, FILE *outfile, mpz_t n) {
    mpz_t i, j; // input, j = output
    mpz_inits(i, j, NULL);
    size_t k;
    //calculated k
    size_t log_n = mpz_sizeinbase(n, 2) - 1; // log_2(sqrt(n))

    k = (0.5 * log_n) / 8; // log_2(sqrt(n)) -1 / 8
    //allocate memory for an array
    uint8_t *array = calloc(k, sizeof(uint8_t));
    //0xFF
    array[0] = 0xFF; // could go inside or outside the while loop

    size_t bytes;
    //while loop
    while (!feof(infile)) {
        bytes = fread(array + 1, sizeof(uint8_t), k - 1, infile);
        mpz_import(i, bytes + 1, 1, sizeof(uint8_t), 0, 0, array);
        ss_encrypt(j, i, n);
        gmp_fprintf(outfile, "%Zx\n", j);
    }
    free(array);
    mpz_clears(i, j, NULL);
}

void ss_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t pq) {
    pow_mod(m, c, d, pq);
}

void ss_decrypt_file(FILE *infile, FILE *outfile, mpz_t d, mpz_t pq) { //works perfectly fine :D
    mpz_t i, j; // input, j = output
    mpz_inits(i, j, NULL);
    size_t k;
    //calculated k
    size_t log_pq = mpz_sizeinbase(pq, 2) - 1; // log_2(sqrt(n))

    k = (log_pq) / 8; // log_2(sqrt(n)) -1 / 8

    //allocate memory for an array
    uint8_t *array = calloc(k, sizeof(uint8_t));

    //0xFF
    array[0] = 0xFF; // could go inside or outside the while loop

    //do-while loop
    while (!feof(infile)) {
        size_t bytes = 0;
        gmp_fscanf(infile, "%Zx\n", i);
        ss_decrypt(j, i, d, pq);
        mpz_export(array, &bytes, 1, sizeof(uint8_t), 1, 0, j);
        fwrite(array + 1, sizeof(uint8_t), bytes - 1, outfile);
    }
    free(array);
    mpz_clears(i, j, NULL);
}
