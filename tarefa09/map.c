/**
 * @file map.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de dicionário usando Burst Trie [HZW02].
 * 
 * Ambas as operações de inserção e busca tomam tempo constante¹. Além disso, o
 * espaço utilizado é aproximadamente o mesmo de uma árvore binária de busca
 * (muito melhor que uma Trie convencional).
 * 
 * Outras propriedades interessantes da burst-trie incluem minimizar a
 * quantidade de operações de alocação de memória em relação a outras
 * estruturas ligadas, que são excepcionalmente custosas, bem como a quantidade
 * de ponteiros derreferenciados, que tem um efeito positivo em desempenho.
 * 
 * Esta implementação não inclui uma operação de remoção, porque não é
 * necessário para a tarefa e os efeitos indesejáveis têm pouco efeito no
 * desempenho geral, de forma que tratar a remoção possivelmente traria mais
 * danos do que benefícios.
 * 
 * ¹ Na realidade, o tempo é proporcional ao tamanho da chave. Como na tarefa
 *   as chaves (nomes) tem tamanho <= 16, é seguro dizer que o tempo gasto é
 *   constante (e uma constante razoávelmente pequena).
 * 
 * [HZW02] HEINZ, Steffen; ZOBEL, Justin; WILLIAMS, Hugh E.
 *         Burst tries: a fast, efficient data structure for string keys.
 *         ACM Transactions on Information Systems (TOIS), v. 20, n. 2,
 *         p. 192-223, 2002. https://doi.org/10.1145/506309.506312
 */

#include "map.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <assert.h>

#include "xmalloc.h"

//=============================================================================
// DEFINIÇÕES (Burst Trie)
//=============================================================================

/**
 * @brief Estrutura de registro chave-valor da burst-trie.
 */
typedef struct {
    char suffix[16];
    double rating; 
} record;

/**
 * @brief Enumeração de tipo de nó da trie de acesso. 
 */
typedef enum {
    TRIE, 
    CONTAINER
} node_type;

/**
 * @brief Estrutura de nó da trie de acesso da burst-trie. 
 * 
 * Um nó de burst-trie pode ser um nó de trie normal, com ponteiros para os
 * filhos e um ponteiro especial para o caractere nulo, ou um nó folha na forma
 * de container.
 * 
 * O container escolhido foi um array, por motivos de localidade de memória e
 * porque na tarefa não há necessidade de lidar com elementos mais ou menos
 * acessados (todos são acessados duas vezes). A heurística usada é a "Limit",
 * que é conveniente porque precisamos do contador de elementos para o array de
 * toda forma. O tamanho do array foi definido para aproveitar o espaço que
 * seria ocupado pelo union de qualquer forma.
 */
typedef struct node {
    node_type type;

    union {
        struct trie {
            double null_leaf;
            struct node* children[26];
        } trie;

        struct container {
            int_fast8_t size;
            record records[8];
        } container;
    };
} node;

typedef struct container container_t;

// Capacidade máxima de um nó-container
#define CONTAINER_CAPACITY (sizeof(((container_t*)0)->records)/sizeof(record))

//=============================================================================
// Implmentação (Burst Trie)
//=============================================================================

/**
 * @brief Aloca e inicializa um nó-folha de container da burst-trie.
 * 
 * Esta função termina o programa com código de erro -1 em caso de falha.
 * 
 * @return ponteiro para o nó alocado.
 */
node* make_container_node() {
    node* c = (node*) xmalloc(sizeof(node));
    c->type = CONTAINER;
    c->container.size = 0;
    return c;
}

/**
 * @brief Busca pelo valor associado a um nome em um container dado.
 * 
 * @param node nó-folha da burst-trie.
 * @param suffix nome sendo buscado.
 * 
 * Esta função assume que a busca será bem sucedida. Caso contrário, seu
 * comportamento é indeterminado.
 * 
 * @return o valor associado ao nome no container. 
 */
double container_find(node* node, const char* suffix) {
    assert(node->type == CONTAINER);
    assert(node->container.size > 0);

    for (int i = node->container.size - 1; i >= 0; i--) {
        record current = node->container.records[i];

        if (strcmp(suffix, current.suffix) == 0) {
            return current.rating;
        }
    }

    // Assumimos que toda busca é bem sucedida (e que vamos usar o GCC rs)
    __builtin_unreachable();
}

/**
 * @brief Adiciona um elemento a um container.
 * 
 * @param node nó-folha da burst-trie.
 * @param suffix sufixo do elemento sendo inserido.
 * @param rating valor associado.
 */
void container_add(node* node, const char* suffix, double rating) {
    assert(node->type == CONTAINER);
    assert(node->container.size < CONTAINER_CAPACITY);

    record* r = &node->container.records[node->container.size];

    strcpy(r->suffix, suffix);
    r->rating = rating;

    node->container.size++;
}

/**
 * @brief Verifica se um container vai "estourar" na próxima inserção.
 * 
 * @param node nó-folha da burst-trie.
 * 
 * @return true caso o container esteja cheio. false se não.
 */
bool container_will_burst(node* node) {
    assert(node->type == CONTAINER);

    return node->container.size == CONTAINER_CAPACITY;
}

/**
 * @brief Associa nome a um valor em uma burst-trie com raíz dada.
 * 
 * @param root raíz da burst-trie.
 * @param name o nome.
 * @param rating o valor associado.
 */
void bursttrie_add(node* root, const char* name, double rating);

/**
 * @brief Associa um sufixo de nome a um valor em um container da burst-trie.
 * 
 * @param node nó do container.
 * @param suffix o sufixo.
 * @param rating o valor associado.
 */
void bursttrie_container_add(node* node, const char* suffix, double rating);

void bursttrie_add(node* root, const char* name, double rating) {
    assert(root->type == TRIE);

    node* current = root;

    const char* suffix = name;

    while (current->type != CONTAINER && suffix[0] != '\0') {
        int key = suffix[0] - 'A';
        node* next = current->trie.children[key];

        if (next == NULL) {
            next = current->trie.children[key] = make_container_node();
            container_add(next, suffix + 1, rating);
            return;
        }

        current = next;
        suffix++;
    }

    if (suffix[0] == '\0') {
        current->trie.null_leaf = rating;
        
    } else {
        bursttrie_container_add(current, suffix, rating);
    }
}

void bursttrie_container_add(node* node, const char* suffix, double rating) {
    assert(node->type == CONTAINER);

    if (container_will_burst(node)) {
        container_t temp;
        memcpy(&temp, &node->container, sizeof(container_t));

        node->type = TRIE;
        memset(&node->trie, 0, sizeof(node->trie));

        for (int i = 0; i < temp.size; i++) {
            record r = temp.records[i];
            bursttrie_add(node, r.suffix, r.rating);
        }

        bursttrie_add(node, suffix, rating);
    } else {
        container_add(node, suffix, rating);
    }
}

void destroy_bursttrie(node* node) {
    if (node->type != TRIE) {
        return;
    }

    for (int i = 0; i < 26; i++) {
        if (node->trie.children[i] != NULL) {
            destroy_bursttrie(node->trie.children[i]);
            free(node->trie.children[i]);
        }
    }
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

struct map {
    node root;
};

map* make_map() {
    map* t = (map*) xmalloc(sizeof(map));
    t->root.type = TRIE;
    memset(&t->root.trie, 0, sizeof(t->root.trie));
    return t;
}

void map_add(map* m, const char* name, double rating) {
    bursttrie_add(&m->root, name, rating);
}

double map_get(map* m, const char* name) {
    node* current = &m->root;

    const char* suffix = name;
    
    while (current->type != CONTAINER && suffix[0] != '\0') {
        current = current->trie.children[suffix[0] - 'A'];
        suffix++;
    }

    if (suffix[0] == '\0' && current->type == TRIE) {
        return current->trie.null_leaf;
    }

    return container_find(current, suffix);
}

void destroy_map(map* m) {
    destroy_bursttrie(&m->root);
    free(m);
}
