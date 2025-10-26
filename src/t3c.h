// En-tête commun pour le programme t3c.

#define _POSIX_C_SOURCE 200809L // pour strdup

#ifndef T3C_H
#define T3C_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

// Entry : couple (password, hash)
typedef struct {
    char *password; // pointeur alloué 
    char *hash;     // pointeur alloué (hex)
} Entry;

/* Table : tableau dynamique d'Entry */
typedef struct {
    Entry *entries;   // tableau d'entrées
    size_t count;     // nombre d'entrées utilisées
    size_t capacity;  // capacité du tableau
} Table;

// hash_utils.c
char *compute_hash_hex(const char *input, const char *algo);

// mode_generate.c
int mode_generate(const char *dict_file, const char *output_file, const char *algo);

// mode_lookup.c
int mode_lookup(const char *table_file, const char *hash_arg);

#endif // T3C_H
