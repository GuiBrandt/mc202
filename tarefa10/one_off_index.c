/**
 * @file one_off_index.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de índice aproximado usando uma BK-Tree.
 */

#include "one_off_index.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <string.h>
#include <math.h>
#include <assert.h>

//=============================================================================
// DEFINIÇÕES
//=============================================================================

// Tamanho máximo de uma palavra no índice
#define KEY_SIZE 25

// Distância de edição máxima (pior caso: 1 remoção e KEY_SIZE substituições)
#define MAX_DISTANCE (KEY_SIZE + 1)

/**
 * @brief Tipo numérico para distâncias de edição. 
 */
typedef uint_fast8_t dist_t;

/**
 * @brief Estrutura de nó de uma BK-Tree.
 * 
 * O nó contém um ponteiro para uma palavra¹ e ponteiros para nós filhos,
 * indexados pela distância de edição entre a palavra deste nó e a palavra do
 * nó filho.
 * 
 * ¹ O ponteiro é recebido na função de inserção e deve permanecer válido
 *   durante toda a vida deste objeto. 
 */
typedef struct bk_node {
    const char* word;
    struct bk_node* children[MAX_DISTANCE];
} bk_node_t;

struct one_off_index {
    bk_node_t* root;  
};

//=============================================================================
// IMPLEMENTAÇÃO (Distância de edição)
//=============================================================================

/**
 * @brief Retorna o menor de dois valores.
 * 
 * @param a um valor.
 * @param b outro valor.
 * 
 * @return o menor valor. 
 */
inline extern size_t min2(size_t a, size_t b) {
    return a < b ? a : b;
}

/**
 * @brief Retorna o menor de três valores.
 * 
 * @param a um valor.
 * @param b outro valor.
 * @param c mais um valor.
 * 
 * @return o menor valor.
 */
inline extern size_t min3(size_t a, size_t b, size_t c) {
    return a < b ? min2(a, c) : min2(b, c);
}

/**
 * @brief Calcula a distância de levenshtein entre duas palavras.
 * 
 * Assume que ambas as palavras têm no máximo 26 caracteres.
 * 
 * @param a uma palavra.
 * @param b outra palavra.
 * 
 * @return lev(a, b), onde:
 *      lev("", b) = len(b)
 *      lev(a, "") = len(a)
 *      lev(a, b) =
 *          se a[0] == b[0]: lev(a[1:], b[1:])         
 *          caso contrário:
 *              1 + min {
 *                  lev(a[1:], b[1:]),      (substituição)
 *                  lev(a[1:], b),          (remoção)
 *                  lev(a, b[1:])           (adição)
 *              }
 */
dist_t edit_distance(const char* a, const char* b) {
    size_t n = strlen(a), m = strlen(b);
    assert(n <= MAX_DISTANCE);
    assert(m <= MAX_DISTANCE);

    // "Buffer duplo" para as linhas necessárias da matriz de DP para cálculo
    // da distância de edição
    dist_t d1[MAX_DISTANCE + 1] = { 0 }, d2[MAX_DISTANCE + 1] = { 0 };
    dist_t *top = d1, *bottom = d2;

    // Caso base 1: "b" vazio
    for (int i = 0; i <= n; i++) {
        bottom[i] = n - i;
    }

    for (int j = m - 1; j >= 0; j--) {
        // Caso base 2: "a" vazio
        top[n] = m - j;

        for (int i = n - 1; i >= 0; i--) {
            if (a[i] == b[j]) {
                top[i] = bottom[i + 1];
            } else {
                dist_t replace = bottom[i + 1],
                       insert = bottom[i],
                       delete = top[i + 1];

                top[i] = min3(replace, insert, delete) + 1;
            }
        }

        dist_t* aux = top;
        top = bottom;
        bottom = aux;
    }

    return bottom[0];
}

//=============================================================================
// IMPLEMENTAÇÃO (Nós da BK-Tree)
//=============================================================================

/**
 * @brief Aloca e inicializa um nó da árvore.
 * 
 * @param word palavra contida no nó.
 * 
 * @return o nó alocado.
 */
bk_node_t* make_node(const char* word) {
    bk_node_t* node = (bk_node_t*) calloc(1, sizeof(bk_node_t));

    if (node == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

    node->word = word;

    return node;
}

/**
 * @brief Libera os recursos alocados para um nó da árvore.
 * 
 * @param node o nó.
 */
void destroy_node(bk_node_t* node) {
    for (int i = 0; i < MAX_DISTANCE; i++) {
        if (node->children[i] != NULL) {
            destroy_node(node->children[i]);
        }
    }
    
    free(node);
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

one_off_index* make_oneoff() {
    one_off_index* oneoff = (one_off_index*) malloc(sizeof(one_off_index));

    if (oneoff == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

    oneoff->root = NULL;

    return oneoff;
}

void oneoff_insert(one_off_index* dict, const char* word) {
    bk_node_t* node = make_node(word);

    if (dict->root == NULL) {
        dict->root = node;
        return;
    }

    bk_node_t *parent, *current = dict->root;
    dist_t d;

    do {
        d = edit_distance(word, current->word);

        // Assumimos que a distância nunca será 0 (i.e. não aceitamos termos
        // repetidos), economizando uma posição no array de nós filhos.
        assert(d != 0);

        parent = current;
        current = current->children[d - 1];
    } while (current != NULL);

    parent->children[d - 1] = node;
}

bool oneoff_query(one_off_index* dict, const char* word) {
    if (dict->root == NULL) {
        return false;
    }

    // Pilha para "recursão", não estoura porque o dicionário tem no máximo
    // 100 palavras.
    bk_node_t* stack[100] = { dict->root, 0 };
    int top = 0;

    dist_t d;
    do {
        bk_node_t* current = stack[top--];

        d = edit_distance(word, current->word);
        assert(d != 0);

        if (d == 1) {
            return true;
        }

        // A busca segue recursivamente para os nós cuja distância de edição
        // para a palavra atual possibilita que tenham uma distância de edição
        // de 1 para a palavra buscada.
        // Neste caso, estes são os nós com distância d - 1, d ou d + 1.
        bk_node_t *a = current->children[d - 2],
                  *b = current->children[d - 1],
                  *c = current->children[d];

        if (a != NULL) stack[++top] = a;
        if (b != NULL) stack[++top] = b;
        if (c != NULL) stack[++top] = c;
    } while (top >= 0);

    return false;
}

void destroy_oneoff(one_off_index* dict) {
    if (dict->root != NULL) {
        destroy_node(dict->root);
    }

    free(dict);
}
