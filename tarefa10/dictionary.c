/**
 * @file dictionary.c
 * @author Guilherme G. Brandt (gui.g.bandt@gmail.com)
 * 
 * @brief Implementação do ADT de dicionário usando Hash table e BK-Tree.
 *
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa10.html 
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

typedef char key_t[25];

typedef struct hash_table {
    size_t size;
    key_t* data;
} hash_table;

struct dict {
    hash_table exact_index;
    one_off_index* approx_index;
};

#define TABLE_SIZE          3

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
        h ^= valid & key[i];
        h *= FNV_prime;
        valid *= key != 0;
    }
    return h;
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

bool hash_query(dict* d, const char* word) {
    hash_t h = hash(word);
    hash_t i = h % TABLE_SIZE, o = i;

    while (d->exact_index.data[i][0] != '\0') {
        if (strcmp(word, d->exact_index.data[i]) == 0) {
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
    d->exact_index.data = (key_t*) calloc(TABLE_SIZE, sizeof(key_t));

    if (d->exact_index.data == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

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
}

color dict_query(dict* d, const char* word) {
    if (hash_query(d, word)) {
        return GREEN;
    //} else if (oneoff_query(d->approx_index, word)) {
    //    return YELLOW;
    } else {
        return RED;
    }
}

void destroy_dict(dict* dict) {
    //destroy_oneoff(dict->approx_index);
    free(dict->exact_index.data);
    free(dict);
}

int main() {
    dict* d = make_dict();

    dict_insert(d, "TEST");
    dict_insert(d, "ASD");
    dict_insert(d, "BFS");

    printf("%d", dict_query(d, "TEST"));
    printf("%d", dict_query(d, "ASD"));
    printf("%d", dict_query(d, "BFS"));

    destroy_dict(d);

    return 0;
}
