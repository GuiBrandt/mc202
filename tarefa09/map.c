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
    mvalue_t value; 
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
            mvalue_t null_leaf;
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
mvalue_t container_find(const node* node, mkey_t suffix) {
    assert(node->type == CONTAINER);
    assert(node->container.size > 0);

    for (int i = node->container.size - 1; i >= 0; i--) {
        record current = node->container.records[i];

        if (strcmp(suffix, current.suffix) == 0) {
            return current.value;
        }
    }

    // Assumimos que toda busca é bem sucedida (e que vamos usar o GCC rs)
    __builtin_unreachable();
}

/**
 * @brief Localiza um elemento em um container. Retorna NULL caso não encontre
 *        o registro do elemento no container.
 * 
 * Os containers são arrays, então a busca é linear, mas como o tamanho deles
 * é reduzido (e o tamanho das chaves deve ser também, além de muito
 * provavelmente a comparação terminar logo no primeiro caractere) o tempo
 * gasto aqui é bem pequeno.
 * 
 * @param node nó-folha da burst-trie.
 * @param suffix sufixo do elemento sendo inserido/atualizado.
 * 
 * @return um ponteiro para o registro do elemento no container, ou NULL caso
 *         não exista.
 */
record* container_locate(node* node, mkey_t suffix) {
    assert(node->type == CONTAINER);
    
    for (int i = 0; i < node->container.size; i++) {
        record* r = &node->container.records[i];
        if (strcmp(r->suffix, suffix) == 0) {
            return r;
        }
    }

    return NULL;
}

/**
 * @brief Adiciona um elemento a um container.
 * 
 * @param node nó-folha da burst-trie.
 * @param r registro no container.
 * @param suffix sufixo do elemento sendo inserido/atualizado.
 * @param value valor associado.
 */
void container_set(node* node, record* r, mkey_t suffix, mvalue_t value) {
    assert(node->type == CONTAINER);

    if (r == NULL) {
        assert(node->container.size < CONTAINER_CAPACITY);
        r = &node->container.records[node->container.size];
        strcpy(r->suffix, suffix);
        node->container.size++;
    }

    r->value = value;
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
 * @param value o valor associado.
 */
void bursttrie_set(node* root, mkey_t name, mvalue_t value);

/**
 * @brief Associa um sufixo de nome a um valor em um container da burst-trie.
 * 
 * @param node nó do container.
 * @param suffix o sufixo.
 * @param value o valor associado.
 */
void bursttrie_container_set(node* node, mkey_t suffix, mvalue_t value);

void bursttrie_set(node* root, mkey_t name, mvalue_t value) {
    assert(root->type == TRIE);

    node* current = root;

    mkey_t suffix = name;

    while (current->type != CONTAINER && suffix[0] != '\0') {
        int key = suffix[0] - 'A';
        node* next = current->trie.children[key];

        if (next == NULL) {
            next = current->trie.children[key] = make_container_node();
            record* r = container_locate(next, suffix);
            container_set(next, r, suffix + 1, value);
            return;
        }

        current = next;
        suffix++;
    }

    if (suffix[0] == '\0') {
        current->trie.null_leaf = value;
        
    } else {
        bursttrie_container_set(current, suffix, value);
    }
}

void bursttrie_container_set(node* node, mkey_t suffix, mvalue_t value) {
    assert(node->type == CONTAINER);

    record* r = container_locate(node, suffix);

    if (r == NULL && container_will_burst(node)) {
        container_t temp;
        memcpy(&temp, &node->container, sizeof(container_t));

        node->type = TRIE;
        memset(&node->trie, 0, sizeof(node->trie));

        for (int i = 0; i < temp.size; i++) {
            record r = temp.records[i];
            bursttrie_set(node, r.suffix, r.value);
        }

        bursttrie_set(node, suffix, value);
    } else {
        container_set(node, r, suffix, value);
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

void map_set(map* m, mkey_t name, mvalue_t value) {
    bursttrie_set(&m->root, name, value);
}

mvalue_t map_get(const map* m, mkey_t name) {
    const node* current = &m->root;

    mkey_t suffix = name;
    
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
