#include <stdio.h> 

#include "numtheory.h" 

#include "ss.h" 

#include <getopt.h> 
#include <gmp.h> 
#include <stdlib.h> 
#include <sys/stat.h> 
#include <unistd.h> 

#define OPTIONS "i:o:n:vh" 

void v(char *user, mpz_t n) { 
    fprintf(stderr, "user = %s\n", user); 
    gmp_fprintf(stderr, "n  (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n); 
}

void usage(void) { 
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Encrypts data using SS encryption.\n");
    fprintf(stderr, "   Encrypted data is decrypted by the decrypt program.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./encrypt [OPTIONS]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -i infile       Input file of data to encrypt (default: stdin).\n");
    fprintf(stderr, "   -o outfile      Output file for encrypted data (default: stdout).\n");
    fprintf(stderr, "   -n pbfile       Public key file (default: ss.pub).\n");
}

int main(int argc, char **argv) {
    int opt = 0; 
    char *public_key_file = "ss.pub"; 
    FILE *infile = stdin; 
    FILE *outfile = stdout; 
    FILE *public_key = NULL; 
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
                fprintf(stderr, "Error opening the file %s or it is not present in the directory\n", optarg); 
                fclose(infile); 
                return EXIT_FAILURE; 
            }
            break; 
        case 'n': 
            public_key_file = optarg; 
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
    public_key = fopen(public_key_file, "r"); 
    if (!public_key) { 
        fprintf(stderr, "Error opening the file %s or it is not present in the directory\n", public_key_file); 
        fclose(infile); 
        fclose(outfile); 
        return EXIT_FAILURE; 
    }
    mpz_t n; 
    mpz_init(n); 
    char *user_name = malloc(1024 * sizeof(char)); 
    ss_read_pub(n, user_name, public_key); 
    ss_encrypt_file(infile, outfile, n); 
    if (verbose) { 
        v(user_name, n); 
    }
    fclose(infile); 
    fclose(outfile); 
    fclose(public_key); 
    mpz_clear(n); 
    free(user_name); 
    return EXIT_SUCCESS; 
}
