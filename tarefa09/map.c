/**
 * @file map.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de dicionário usando Trie.
 */

#include "map.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <string.h>

#include "util.h"

struct map {
    double value;
    struct map* children[26];
};

/**
 * @brief Obtém o ponteiro para a próxima trie dada uma chave.
 * 
 * Esta função assume que a trie para a chave dada existe no dicionário, e seu
 * comportamento é indeterminado caso contrário.
 * 
 * @param t o dicionário.
 * @param key a chave.
 * 
 * @return ponteiro para o próximo nó da trie.
 */
map* trie_next(map* t, char key) {
    return t->children[key - 'A'];
}

/**
 * @brief Obtém ou aloca o ponteiro para a próxima trie dada uma chave.
 * 
 * Esta função aloca a próxima trie caso necessário. Em caso de falha, ela
 * termina o programa com código de erro -1.
 * 
 * @param t o dicionário.
 * @param c a chave.
 * 
 * @return ponteiro para o próximo nó da trie.
 */
map* trie_next_or_create(map* t, char c) {
    int key = c - 'A';

    if (t->children[key] == NULL) {
        t->children[key] = make_map();
    }

    return t->children[key];
}

map* make_map() {
    map* t = (map*) xmalloc(sizeof(map));
    memset(t->children, 0, 26 * sizeof(map*));
    return t;
}

void map_add(map* t, const char* name, double rating) {
    while (name[0] != '\0') {
        t = trie_next_or_create(t, *name);
        name++;
    }

    t->value = rating;
}

double map_get(map* t, const char* name) {
    while (name[0] != '\0') {
        t = trie_next(t, *name);
        name++;
    }

    return t->value;
}

void destroy_map(map* t) {
    for (int i = 0; i < 26; i++) {
        if (t->children[i] == NULL) {
            continue;
        }

        destroy_map(t->children[i]);
    }

    free(t);
}
