#include <stdio.h>

#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <getopt.h>
#include <gmp.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define OPTIONS "b:i:n:d:svh" 

void verbose(char *user, mpz_t p, mpz_t q, mpz_t pq, mpz_t n, mpz_t d) {
    fprintf(stderr, "user = %s\n", user); 
    gmp_fprintf(stderr, "p  (%zu bits) = %Zd\n", mpz_sizeinbase(p, 2), p); 
    gmp_fprintf(stderr, "q  (%zu bits) = %Zd\n", mpz_sizeinbase(q, 2), q); 
    gmp_fprintf(stderr, "n  (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n); 
    gmp_fprintf(stderr, "pq (%zu bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq); 
    gmp_fprintf(stderr, "d  (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d); 
}

void usage(void) { 
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Generates an SS public/private key pair.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./keygen [OPTIONS]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -b bits         Minimum bits needed for public key n (default: 256).\n");
    fprintf(stderr, "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n");
    fprintf(stderr, "   -n pbfile       Public key file (default: ss.pub).\n");
    fprintf(stderr, "   -d pvfile       Private key file (default: ss.priv).\n");
    fprintf(stderr, "   -s seed         Random seed for testing.\n");
}

int main(int argc, char **argv) {
    int opt = 0; 
    uint64_t bits = 256; 
    uint64_t iters = 50; 
    char *public_file_name = "ss.pub"; 
    char *private_file_name = "ss.priv"; 
    uint64_t seed = (uint64_t) time(NULL); 
    char *user_name = getenv("USER"); 
    bool verbose_flag = false; 
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) { 
        switch (opt) { 
        case 'b': 
            if (atoi(optarg) < 5) { 
                fprintf(stderr, "Floating point exception\n"); 
                return EXIT_FAILURE; 
            } else {
                bits = atoi(optarg); 
            }
            break; 
        case 'i': 
            if (atoi(optarg) < 0) { 
                fprintf(stderr, "Number of iteration cannot be negative\n"); 
                return EXIT_FAILURE; 
            } else {
                iters = atoi(optarg); 
            }
            break; 
        case 'n': 
            public_file_name = optarg; 
            break; 
        case 'd':
            private_file_name = optarg; 
            break; 
        case 'v': 
            verbose_flag = true; 
            break; 
        case 's': 
            seed = atoi(optarg); 
            break; 
        case 'h': 
            usage(); 
            return EXIT_SUCCESS; 
        default: 
            usage(); 
            return EXIT_FAILURE; 
        }
    }
    FILE *pbfile = fopen(public_file_name, "w"); 
    if (!pbfile) { 
        fprintf(stderr, "Error opening the file %s or it is not in the directory\n", public_file_name); 
        return EXIT_FAILURE; 
    }
    FILE *pvfile = fopen(private_file_name, "w"); 
    if (!pvfile) { 
        fprintf(stderr, "Error opening the file %s or it is not in the directory\n", private_file_name); 
        fclose(pbfile); 
        return EXIT_FAILURE; 
    }
    fchmod(fileno(pvfile), 0600); 
    randstate_init(seed); 
    mpz_t p, q, n, d, pq;
    mpz_inits(p, q, n, d, pq, NULL); 
    ss_make_pub(p, q, n, bits, iters); 
    ss_make_priv(d, pq, p, q); 
    if (verbose_flag) { 
        verbose(user_name, p, q, pq, n, d); 
    }
    ss_write_pub(n, user_name, pbfile); 
    ss_write_priv(pq, d, pvfile); 
    fclose(pbfile); 
    fclose(pvfile); 
    mpz_clears(p, q, n, d, pq, NULL); 
    randstate_clear(); 
    return EXIT_SUCCESS; 
}
