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
        "   Encrypts data using SS encryption.\n"
        "   Encrypted data is decrypted by the decrypt program.\n"
        "\n"
        "USAGE\n"
        "  %s [OPTIONS]\n"
        "\n"
        "OPTIONS\n"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -i infile       Input file of data to encrypt (default: stdin).\n"
        "   -o outfile      Output file for encrypted data (default: stdout).\n"
        "   -n pvfile       Private key file (default: ss.pub).\n",
        exec);
}

int main(int argc, char **argv) {
    int opt = 0;
    FILE *inpfile = stdin;
    FILE *opfile = stdout;
    FILE *pub_key = fopen("ss.pub", "r");
    bool verbose = false;
    mpz_t pub;
    mpz_init(pub);
    char *username = getenv("USER");

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

        case 'n':
            pub_key = fopen(optarg, "r"); //reads an input file
            if (pub_key == NULL) { //supposed to error handle..
                printf("%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;

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

    ss_read_pub(pub, username, pub_key);
    uint64_t pbits = mpz_sizeinbase(pub, 2);

    if (verbose == true) {
        fprintf(stderr, "user = %s\nn (%lu bits) = ", username, pbits);
        gmp_fprintf(stderr, "%Zd\n", pub);
    }

    if (pbits < 33) {
        return 0;
    }

    ss_encrypt_file(inpfile, opfile, pub);
    fclose(pub_key);
    fclose(inpfile);
    fclose(opfile);
    mpz_clear(pub);
}
