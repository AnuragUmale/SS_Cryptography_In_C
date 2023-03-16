#include <stdio.h> 

#include "numtheory.h" 
#include "randstate.h" 
#include "ss.h" 

#include <gmp.h> 
#include <stdbool.h> 
#include <stdint.h> 
#include <stdlib.h> 
#include <sys/types.h> 

#define EPSILON 1e-14 

static inline double absolute(double x) { 
    return x < 0.0 ? -x : x; 
}

static inline double sqrt_newton(double x) { 
    double z = 0.0; 
    double y = 1.0; 
    while (absolute(y - z) > EPSILON) { 
        z = y; 
        y = 0.5 * (z + (x / z)); 
    }
    return y;
}

static inline uint64_t log_base_2(double x) { 
    uint64_t k = -1; 
    uint64_t n = absolute(x); 
    while (n > 0) { 
        n /= 2.0; 
        k += 1; 
    }
    return k; 
}

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    uint64_t upper_limit = (2 * nbits) / 5; 
    uint64_t lower_limit = nbits / 5; 
    uint64_t log2 = 0; 
    uint64_t p_bits = (random() % (upper_limit - lower_limit + 1)) + lower_limit; 
    uint64_t q_bits = (nbits - (2 * p_bits)); 
    mpz_t tmp, temp_n; 
    mpz_inits(tmp, temp_n, NULL); 
    while (1) {
        make_prime(p, p_bits, iters); 
        make_prime(q, q_bits, iters); 
        mpz_sub_ui(tmp, q, 1); 
        mpz_mul(temp_n, p, p); 
        mpz_mul(temp_n, temp_n, q); 
        log2 = mpz_sizeinbase(temp_n, 2); 
        if ((mpz_cmp(tmp, p) == 0 || mpz_cmp_ui(tmp, 0)) && log2 >= nbits) { 
            break; 
        }
        mpz_set_ui(temp_n, 0); 
    }
    mpz_mul(n, p, p); 
    mpz_mul(n, n, q); 
    mpz_clears(tmp, temp_n, NULL); 
    return; 
}

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%s\n", n, username); 
    return; 
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx", n); 
    gmp_fscanf(pbfile, "%s", username); 
    return; 
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t gcd_val, n, p_minus_1, q_minus_1, temp; 
    mpz_inits(gcd_val, n, p_minus_1, q_minus_1, temp, NULL); 
    mpz_sub_ui(p_minus_1, p, 1); 
    mpz_sub_ui(q_minus_1, q, 1); 
    gcd(gcd_val, p_minus_1, q_minus_1); 
    mpz_mul(temp, p_minus_1, q_minus_1); 
    mpz_div(temp, temp, gcd_val); 
    mpz_mul(pq, p, q); 
    mpz_mul(n, p, pq); 
    mod_inverse(d, n, temp); 
    mpz_clears(gcd_val, n, p_minus_1, q_minus_1, temp, NULL); 
    return; 
}

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d); 
    return; 
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", pq); 
    gmp_fscanf(pvfile, "%Zx", d); 
    return; 
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n); 
    return; 
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    size_t k = 0; 
    int buffer = 0; 
    mpz_t m, c; 
    mpz_inits(m, c, NULL); 
    size_t square_root = sqrt_newton(mpz_get_d(n)); 
    size_t log2 = log_base_2(square_root); 
    k = (log2 - 1) / 8; 
    uint8_t *block = (uint8_t *) malloc(k * sizeof(uint8_t)); 
    if (!block) { 
        gmp_fprintf(stderr, "Error allocating the memory\n"); 
        exit(1); 
    }
    block[0] = 0xFF; 
    while ((buffer = fgetc(infile)) != EOF) { 
        size_t j = 1; 
        block[j] = buffer; 
        j += 1; 
        while (j < k) { 
            if ((buffer = fgetc(infile)) != EOF) { 
                block[j] = buffer; 
                j += 1; 
            }
            break;
        }
        mpz_import(m, j, 1, sizeof(block[j]), 1, 0, block); 
        ss_encrypt(c, m, n); 
        gmp_fprintf(outfile, "%Zx\n", c); 
    }
    mpz_clears(m, c, NULL); 
    free(block);
    return; 
}

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq); 
    return; 
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    size_t k = 0; 
    mpz_t c, m; 
    mpz_inits(c, m, NULL); 
    k = (log_base_2(mpz_get_d(pq)) - 1) / 8; 
    uint8_t *array = (uint8_t *) malloc(k * sizeof(uint8_t)); 
    while (gmp_fscanf(infile, "%Zx", c) != EOF) { 
        size_t j = 1; 
        ss_decrypt(m, c, d, pq); 
        mpz_export(array, &k, 1, sizeof(array[0]), 1, 0, m); 
        while (j < k) { 
            gmp_fprintf(outfile, "%c", array[j]); 
            j += 1; 
        }
    }
    mpz_clears(c, m, NULL); 
    free(array); 
    return; 
}
