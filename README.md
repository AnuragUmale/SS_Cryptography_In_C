# Schmidt-Samoa (SS) - Public Key Cryptography
**Public key cryptography** is a technique for secure communication using a pair of keys: one **public** and one **private**.

## Files In The Repository

1. Makefile
2. DESIGN.pdf
3. WRITEUP.pdf
4. README.md
5. ss.h
6. ss.c
7. randstate.h
8. randstate.c
9. numtheory.h
10. numtheory.c
11. keygen.c
12. encrypt.c
13. decrypt.c

# Make commands

```
$ make clean
```

This command will make the repository clean by deleting the object file and executable generated.


```
$ make format
```

This command will format C file into clang format.

```
$ make
```

This command will just compile other C files and make the executable and object file for **keygen.c**,**encrypt.c** and **decrypt.c**.

## Running the program

```
$ make
```

1. This command will just compile and make the executable and object file for **keygen.c**,**encrypt.c** and **decrypt.c**..

```
$ ./keygen -flag
```

2. flags = b,i,n,d,s,v and h you can pass one or many flags together.

### Flags

-b {bits}
:  Minimum bits needed for public key n (default: 256).

-i {iteration}
: Miller-Rabin iterations for testing primes (default: 50).

-n {pbfile}
:  Public key file (default: ss.pub).

-d {pvfile}
:  Public key file (default: ss.priv).

-s {seed}
:Random seed for testing.

-v {}
: Display verbose program output.

-h
:Display program help and usage.

```
$./encrypt -flags
```

3. flags = i,o,n,v and h.

### Flags

-i {infile}
: Input file of data to encrypt (default: stdin).

-o {outfile}
: Output file for encrypted data (default: stdout).

-n {pbfile}
: Public key file (default: ss.pub).

-v
: Display verbose program output.

-h
: Display program help and usage.



```
$./decrypt -flag
```


### Flags

-i {infile}
: Input file of data to encrypt (default: stdin).

-o {outfile}
: Output file for encrypted data (default: stdout).

-n {pbfile}
: Public key file (default: ss.priv).

-v
: Display verbose program output.

-h
: Display program help and usage.


# Note
If you are a student and if are taking a look at my repository for your assignment, any piece of code that your copy from this repository is completely your responsibility. I should not be held repsonsible for any academic misconduct.