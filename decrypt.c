#include <stdio.h> 
#include "numtheory.h" 
#include "randstate.h" 
#include "ss.h" 

#include <getopt.h> 
#include <gmp.h> 
#include <stdlib.h> 
#include <sys/stat.h> 
#include <unistd.h> 

#define OPTIONS "i:o:n:vh" 

void v(mpz_t pq, mpz_t d) {
    gmp_fprintf(stderr, "pq  (%zu bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq); 
    gmp_fprintf(stderr, "d  (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d); 
}

void usage(void) { 
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Decrypts data using SS decryption.\n");
    fprintf(stderr, "   Encrypted data is decrypted by the decrypt program.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./decrypt [OPTIONS]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -i infile       Input file of data to encrypt (default: stdin).\n");
    fprintf(stderr, "   -o outfile      Output file for encrypted data (default: stdout).\n");
    fprintf(stderr, "   -n pbfile       Public key file (default: ss.pub).\n");
}

int main(int argc, char **argv) {
    int opt = 0; 
    char *private_key_file = "ss.priv"; 
    FILE *infile = stdin; 
    FILE *outfile = stdout; 
    FILE *private_key = NULL; 
    bool verbose = false; 
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) { 
        switch (opt) {
        case 'i': 
            infile = fopen(optarg, "r"); 
            if (!infile) { 
                fprintf(stderr, "Error opening the file %s or it is not present in the directory\n", optarg); 
                return EXIT_FAILURE; 
            }
            break; 
        case 'o': 
            outfile = fopen(optarg, "w"); 
            if (!outfile) { 
                fprintf(stderr, "Error opening the file %s or it is not present in the directory\n",optarg); 
                fclose(infile); 
                return EXIT_FAILURE; 
            }
            break; 
        case 'n': 
            private_key_file = optarg; 
            break; 
        case 'v': 
            verbose = true; 
            break; 
        case 'h': 
            usage(); 
            return EXIT_SUCCESS; 
        default: 
            usage(); 
            return EXIT_FAILURE; 
        } 
    } 
    private_key = fopen(private_key_file, "r"); 
    if (!private_key) { 
        fprintf(stderr, "Error opening the file %s or it is not present in the directory\n", private_key_file); 
        fclose(infile); 
        fclose(outfile); 
        return EXIT_FAILURE; 
    }
    mpz_t pq, d; 
    mpz_inits(pq, d, NULL); 
    ss_read_priv(pq, d, private_key); 
    ss_decrypt_file(infile, outfile, d,pq); 
    if (verbose) { 
        v(pq,d); 
    }
    fclose(infile); 
    fclose(outfile); 
    fclose(private_key); 
    mpz_clears(pq, d, NULL); 
    return EXIT_SUCCESS; 
}
