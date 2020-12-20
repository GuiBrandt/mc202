/**
 * @file pokedisjointset.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de união-busca (disjoint-set).
 * 
 * A estrutura é aumentada para permitir marcar conjuntos, mecanismo utilizado
 * para marcar conjuntos onde um Lugia existe no algoritmo de caminho com menor
 * distância máxima.
 * 
 * A marcação é mantida da mesma forma que os tamanhos das árvores, apenas
 * trocando o monóide dos naturais pelo monóide booleano em "ou".
 */

#include "pokedisjointset.h"

#include <stdlib.h>
#include <stdbool.h>

#include "xmalloc.h"

//=============================================================================
// DEFINIÇÕES
//=============================================================================

struct disjoint_set {
    int* parents;
    size_t* sizes;
    bool* marks;
};

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

disjoint_set_t* make_disjoint_set(size_t capacity) {
    disjoint_set_t* ds = (disjoint_set_t*) xmalloc(sizeof(disjoint_set_t));
    
    ds->parents = (int*) xmalloc(sizeof(int) * capacity);
    ds->sizes = (size_t*) xmalloc(sizeof(size_t) * capacity);
    ds->marks = (bool*) xmalloc(sizeof(bool) * capacity);

    for (int i = 0; i < capacity; i++) {
        ds->parents[i] = i;
        ds->sizes[i] = 1;
        ds->marks[i] = false;
    }

    return ds;
}

// O(α(n)) amortizado
int disjoint_set_find(disjoint_set_t* ds, int element) {
    int current = element;
    
    while (ds->parents[current] != current) {
        ds->parents[current] = ds->parents[ds->parents[current]];
        current = ds->parents[current];
    }

    return current;
}

// O(α(n)) amortizado
void disjoint_set_mark(disjoint_set_t* ds, int element) {
    ds->marks[disjoint_set_find(ds, element)] = true;
}

// O(α(n)) amortizado
bool disjoint_set_marked(disjoint_set_t* ds, int element) {
    return ds->marks[disjoint_set_find(ds, element)];
}

// O(α(n)) amortizado
void disjoint_set_merge(disjoint_set_t* ds, int a, int b) {
    int root_a = disjoint_set_find(ds, a),
        root_b = disjoint_set_find(ds, b);

    if (root_a == root_b) {
        return;
    }

    // Troca as raízes para controlar a altura da árvore caso necessário
    if (ds->sizes[root_b] > ds->sizes[root_a]) {
        root_a ^= root_b;
        root_b ^= root_a;
        root_a ^= root_b;
    }

    ds->parents[root_b] = root_a;
    ds->sizes[root_a] += ds->sizes[root_b];
    ds->marks[root_a] |= ds->marks[root_b];
}

void destroy_disjoint_set(disjoint_set_t* ds) {
    free(ds->parents);
    free(ds);
}