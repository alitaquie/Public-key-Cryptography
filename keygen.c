#include "randstate.h"
#include "ss.h"
#include "numtheory.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#define OPTIONS "vb:i:n:d:s:h"
#include <gmp.h>

void usage(char *exec) { //usage function message
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Generates an SS public/private key pair\n"
        "\n"
        "USAGE\n"
        "  %s [OPTIONS]\n"
        "\n"
        "OPTIONS\n"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -b bits     Minimum bits needed for public key n (default: 256).\n"
        "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n"
        "   -n pbfile       Public key file (default: ss.pub).\n"
        "   -d pvfile       Private key file (default: ss.priv).\n"
        "   -s seed         Random seed for testing.\n",
        exec);
}

int main(int argc, char **argv) {
    int seed = time(NULL);
    int opt = 0;
    ;
    FILE *pbfile = fopen("ss.pub", "w");
    FILE *pvfile = fopen("ss.priv", "w");
    uint64_t iters = 50;
    uint64_t bits = 256;
    bool verbose = false;
    mpz_t p, q, n, d, pq;
    mpz_inits(p, q, n, d, pq, NULL);

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': bits = strtoul(optarg, NULL, 10); break;

        case 'i': iters = strtoul(optarg, NULL, 10); break;

        case 'n':
            pbfile = fopen(optarg, "w"); //reads an input file
            if (pbfile == NULL) { //supposed to error handle..
                printf("Error opening %s \n", optarg);
                return EXIT_FAILURE;
            }
            break;

        case 'd':
            pvfile = fopen(optarg, "w"); //reads an input file
            if (pvfile == NULL) { //supposed to error handle..
                printf("Error opening %s \n", optarg);
                return EXIT_FAILURE;
            }
            break;

        case 's': seed = strtoul(optarg, NULL, 10); break;

        case 'v': verbose = true; break;

        case 'h':
            usage(argv[0]); //displays help message
            return EXIT_FAILURE;
            break;

        default:
            usage(argv[0]); //displays help message
            return EXIT_FAILURE;
            break;
        }
    }

    fchmod(fileno(pvfile), 0600);
    randstate_init(seed);
    ss_make_pub(p, q, n, bits, iters);
    ss_make_priv(d, pq, p, q);
    char *username = getenv("USER");
    ss_write_pub(n, username, pbfile);
    ss_write_priv(pq, d, pvfile);

    if (verbose == true) {
        uint64_t pbits = mpz_sizeinbase(p, 2);
        uint64_t qbits = mpz_sizeinbase(q, 2);
        uint64_t nbits = mpz_sizeinbase(n, 2);
        uint64_t pqbits = mpz_sizeinbase(pq, 2);
        uint64_t dbits = mpz_sizeinbase(d, 2);

        printf("user = %s\np (%lu bits) = ", username, pbits);
        gmp_printf("%Zd\n", p);
        printf("q (%lu bits) = ", qbits);
        gmp_printf("%Zd\n", q);
        printf("n (%lu bits) = ", nbits);
        gmp_printf("%Zd\n", n);
        printf("pq (%lu bits) = ", pqbits);
        gmp_printf("%Zd\n", pq);
        printf("d (%lu bits) = ", dbits);
        gmp_printf("%Zd\n", d);
    }

    randstate_clear();
    mpz_clears(p, q, n, d, pq, NULL);
}
