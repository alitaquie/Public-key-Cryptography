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
#define OPTIONS "i:o:n:vh"
#include <gmp.h>

void usage(char *exec) { //usage function message
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Decrypts data using SS decryption.\n"
        "   Encrypted data is encrypted by the encrypt program.\n"
        "\n"
        "USAGE\n"
        "  %s [OPTIONS]\n"
        "\n"
        "OPTIONS\n"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -i infile       Input file of data to decrypt (default: stdin).\n"
        "   -o outfile      Output file for decrypted data (default: stdout).\n"
        "   -n pvfile       Private key file (default: ss.priv).\n",
        exec);
}

int main(int argc, char **argv) {
    int opt = 0;
    FILE *inpfile = stdin;
    FILE *opfile = stdout;
    char *priv_key = "ss.priv";
    FILE *priv_file;
    bool verbose = false;
    mpz_t priv, pq;
    mpz_inits(priv, pq, NULL);

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            inpfile = fopen(optarg, "r");
            if (inpfile == NULL) {
                printf("%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;

        case 'o':
            opfile = fopen(optarg, "w"); //reads an input file
            if (opfile == NULL) { //supposed to error handle..
                printf("%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;

        case 'n': priv_key = optarg; break;

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

    priv_file = fopen(priv_key, "r");
    if (priv_file == NULL) {
        return EXIT_FAILURE;
    }
    ss_read_priv(pq, priv, priv_file);

    uint64_t pbits = mpz_sizeinbase(priv, 2);
    uint64_t pqbits = mpz_sizeinbase(pq, 2);

    if (verbose == true) {
        fprintf(stderr, "pq (%lu bits) = ", pqbits);
        gmp_fprintf(stderr, "%Zd\n", pq);
        fprintf(stderr, "d (%lu bits) = ", pbits);
        gmp_fprintf(stderr, "%Zd\n", priv);
    }

    ss_decrypt_file(inpfile, opfile, priv, pq);
    fclose(priv_file);
    fclose(opfile);
    fclose(inpfile);
    mpz_clears(priv, pq, NULL);
}
