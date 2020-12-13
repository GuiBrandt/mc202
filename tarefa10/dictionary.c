/**
 * @file dictionary.c
 * @author Guilherme G. Brandt (gui.g.bandt@gmail.com)
 * 
 * @brief Implementação do ADT de dicionário usando Hash table, extendido para
 *        busca aproximada usando o ADT de dicionário de distância de edição 1.
 */

#include "dictionary.h"

#include <string.h>
#include <assert.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "one_off_index.h"

//=============================================================================
// DEFINIÇÕES
//=============================================================================

typedef uint32_t hash_t;

typedef char key_t[26];

typedef struct hash_table {
    size_t size;
    key_t* data;
} hash_table;

struct dict {
    hash_table exact_index;
    one_off_index* approx_index;
};

#define TABLE_SIZE 233

//=============================================================================
// IMPLEMENTAÇÃO (Hashing)
//=============================================================================

#define FNV_prime           0x01000193 
#define FNV_offset_basis    0x811c9dc5

/**
 * @brief Calcula o valor do hash para uma chave dada.
 * 
 * @param key a chave.
 * 
 * @return o hash.
 * 
 * @see http://isthe.com/chongo/tech/comp/fnv/#FNV-1a
 */
hash_t hash(const char* key) {
    hash_t h = FNV_offset_basis;
    hash_t valid = ~0;
    for (size_t i = 0; i < 25; i++) {
        if (valid) {
            h ^= valid & key[i];
            valid *= key[i] != 0;
        }

        h *= FNV_prime;
    }
    return h;
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

bool hash_query(hash_table* table, const char* word) {
    hash_t h = hash(word);
    hash_t i = h % TABLE_SIZE, o = i;

    while (table->data[i][0] != '\0') {
        if (strcmp(word, table->data[i]) == 0) {
            return true;
        }

        i = (i + 1) % TABLE_SIZE;
        if (i == o) break;
    }

    return false;
}

dict* make_dict() {
    dict* d = (dict*) malloc(sizeof(dict));

    if (d == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

    d->exact_index.size = TABLE_SIZE;
    d->exact_index.data = (key_t*) malloc(TABLE_SIZE * sizeof(key_t));

    if (d->exact_index.data == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        d->exact_index.data[i][0] = '\0';
    }

    d->approx_index = make_oneoff();

    return d;
}

void dict_insert(dict* d, const char* word) {
    hash_t h = hash(word);
    hash_t i = h % TABLE_SIZE, o = i;

    while (d->exact_index.data[i][0] != '\0') {
        i = (i + 1) % TABLE_SIZE;
        assert(i != o);
    }

    strcpy(d->exact_index.data[i], word);
    oneoff_insert(d->approx_index, d->exact_index.data[i]);
}

color dict_query(dict* d, const char* word) {
    if (hash_query(&d->exact_index, word)) {
        return GREEN;
    } else if (oneoff_query(d->approx_index, word)) {
        return YELLOW;
    } else {
        return RED;
    }
}

void destroy_dict(dict* dict) {
    destroy_oneoff(dict->approx_index);
    free(dict->exact_index.data);
    free(dict);
}
