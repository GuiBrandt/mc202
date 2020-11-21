/**
 * @file tree_multiset.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de multiset usando uma árvore Splay.
 *
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa08.html
 */

#include "tree_multiset.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

/**
 * @brief Estrutura de nó da árvore splay. 
 */
typedef struct tree_multiset_node {
    // Chave do nó
    element_t key;

    // Número de repetições no conjunto
    size_t count;

    // Número de remoções necessários na subárvore com raíz no nó para tornar a
    // árvore legal
    size_t diff_cool;

    // Filho esquerdo
    struct tree_multiset_node* left;

    // Filho direito
    struct tree_multiset_node* right;

    // Nó pai (NULL para a raíz)
    struct tree_multiset_node* parent;
} node;

struct tree_multiset {
    node* root;
};

/**
 * @brief Corrige os ponteiros para os pais dos nós após uma rotação.
 * 
 * @param o pai do nó rotacionado.
 * @param p nó rotacionado.
 * @param q filho rotacionado.
 * @param y filho trocado (na rotação direita, o filho direito de q, na
 *          esquerda o filho esquerdo de q).
 */
void maintain_parents(node* o, node* p, node* q, node* y) {
    assert(p != NULL);
    assert(q != NULL);

    if (o != NULL) {
        if (o->right == p) {
            o->right = q;
        } else {
            o->left = q;
        }
    }

    if (y != NULL) {
        y->parent = p;
    }

    p->parent = q;
    q->parent = o;
}

/**
 * @brief Faz uma rotação para a esquerda em um nó.
 * 
 * A rotação segue o diagrama abaixo:
 * 
 *       o              o
 *       |              |
 *       P              Q
 *      / \            / \
 *     x   Q    =>    P   z
 *        / \        / \
 *       y   z      x   y
 * 
 * Onde P é o nó sendo rotacionado. Os nós em maiúscula são obrigatórios (i.e.
 * não podem ser NULL).
 * 
 * @param root o nó sendo rotacionado.
 */
void rotate_left(node* p)
    __attribute__((hot));

void rotate_left(node* p) {
    assert(p != NULL);
    assert(p->right != NULL);

    node* o = p->parent;
    node* q = p->right;
    node* y = q->left;
    
    q->left = p;
    p->right = y;

    if (y != NULL) {
        q->diff_cool -= y->diff_cool;
    }

    p->diff_cool -= q->diff_cool;
    q->diff_cool += p->diff_cool;

    maintain_parents(o, p, q, y);
}

/**
 * @brief Faz uma rotação para a direita em um nó.
 * 
 * A rotação segue o diagrama abaixo:
 * 
 *       o              o
 *       |              |
 *       P              Q
 *      / \            / \
 *     Q   z    =>    x   P
 *    / \                / \
 *   x   y              y   z
 * 
 * Onde P é o nó sendo rotacionado. Os nós em maiúscula são obrigatórios (i.e.
 * não podem ser NULL).
 * 
 * @param root o nó sendo rotacionado.
 */
void rotate_right(node* p)
    __attribute__((hot));

void rotate_right(node* p) {
    assert(p != NULL);
    assert(p->left != NULL);

    node* o = p->parent;
    node* q = p->left;
    node* y = q->right;

    q->right = p;
    p->left = y;

    if (y != NULL) {
        q->diff_cool -= y->diff_cool;
    }

    p->diff_cool -= q->diff_cool;
    q->diff_cool += p->diff_cool;

    maintain_parents(o, p, q, y);
}

/**
 * @brief Faz splay de um nó até que esteja na raíz.
 * 
 * @param subject nó sendo rotacionado.
 */
void splay(tree_multiset* multiset, node* subject)
    __attribute__((hot));

void splay(tree_multiset* multiset, node* subject) {
    while (subject->parent != NULL) {
        node* parent = subject->parent;
        node* grandparent = parent->parent;

        assert(parent != NULL);

        if (parent->right == subject) {
            // Zag (Rotação para a esquerda)
            if (grandparent == NULL) {
                rotate_left(parent);
                break;
            }

            assert(grandparent != NULL);

            /* Zag-Zag (em R)
             *
             *    P                      R
             *   / \                    / \ 
             *  x   Q                  Q   w
             *     / \       =>       / \ 
             *    y   R              P   z
             *       / \            / \ 
             *      z   w          x   y
             */
            if (grandparent->right == parent) {
                rotate_left(grandparent);
                rotate_left(parent);

            /* Zag-Zig (em P)
             *
             *        R 
             *       / \               P
             *      Q   w            /   \ 
             *     / \       =>     Q     R
             *    x   P            / \   / \ 
             *       / \          x   y z   w
             *      y   z
             */
            } else {
                assert(grandparent->left == parent);

                rotate_left(parent);
                rotate_right(grandparent);
            }

        } else {
            assert(parent->left == subject);

            // Zig (Rotação para a direita)
            if (grandparent == NULL) {
                rotate_right(parent);
                break;
            }

            assert(grandparent != NULL);

            // Zig-Zag (Simétrico a Zag-Zig)
            if (grandparent->right == parent) {
                rotate_right(parent);
                rotate_left(grandparent);

            // Zig-Zig (Inverso de Zag-Zag)
            } else {
                assert(grandparent->left == parent);

                rotate_right(grandparent);
                rotate_right(parent);
            }
        }
    }

    multiset->root = subject;
}

/**
 * @brief Aloca memória e termina o programa com uma mensagem em caso de falha.
 * 
 * @param size número de bytes a serem alocados.
 * @return o ponteiro alocado.
 */
void* xmalloc(size_t size)
    __attribute_malloc__;

void* xmalloc(size_t size) {
    void* ptr = malloc(size);

    if (ptr == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.\n");
        exit(-1);
    }

    return ptr;
}

/**
 * @brief Constrói um nó-folha com a chave dada e count = 1.
 * 
 * @param key chave do nó.
 * 
 * @return ponteiro para o nó alocado.
 */
node* make_node(element_t key) {
    node* v = (node*) xmalloc(sizeof(node));
    v->key = key;
    v->count = 1;
    v->diff_cool = key > 1 ? 1 : 0;
    v->left = NULL;
    v->right = NULL;
    v->parent = NULL;
    return v;
}

/**
 * @brief Libera a memória alocada para um nó e seus filhos.
 * 
 * @param root nó raíz.
 */
void destroy_node(node* root) {
    if (root == NULL) {
        return;
    }
    
    destroy_node(root->left);
    destroy_node(root->right);

    free(root);
}

/**
 * @brief Corrige o valor de diferença-legal para um nó na árvore.
 * 
 * Essa função assume que os valores de diferença-legal para as subárvores do
 * nó dado estão corretos.
 * 
 * @param v nó cujo valor de diferença-legal deve ser corrigido.
 */
void maintain_diff_cool(node* v) {    
    if (v->count >= v->key) {
        v->diff_cool = v->count - v->key;
    } else {
        v->diff_cool = v->count;
    }

    if (v->left != NULL) {
        v->diff_cool += v->left->diff_cool;
    }
    
    if (v->right != NULL) {
        v->diff_cool += v->right->diff_cool;
    }
}

/**
 * @brief Propaga uma alteração no valor da diferença-legal de um nó para cima
 *        na árvore.
 * 
 * @param found nó cujo valor de diferença-legal mudou (possivelmente um nó que
 *              acaba de ser inserido na árvore).
 */
void propagate_diff_cool(node* found) {
    node* current = found->parent;

    while (current != NULL) {
        maintain_diff_cool(current);
        current = current->parent;
    }
}

/**
 * @brief Encontra o nó de um elemento fazendo busca na árvore binária.
 * 
 * @param root raíz da árvore.
 * @param key valor sendo procurado.
 * @param parent ponteiro de saída para o nó pai do nó do valor buscado.
 * 
 * Se o valor não estiver presente na árvore, parent recebe o ponteiro para o
 * nó sob o qual deve-se inserir elemento.
 * 
 * @return um ponteiro para o nó encontrado, ou NULL caso não exista.
 */
node* find(node* root, element_t key, node** parent) {
    node* current = root;
    *parent = NULL;

    while (current != NULL) {
        *parent = current;

        if (key < current->key) {
            current = current->left;
        } else if (current->key == key) {
            return current;
        } else {
            current = current->right;
        }
    }

    return NULL;
}

//=============================================================================
// Contrato
//=============================================================================

tree_multiset* multiset_init() {
    tree_multiset* multiset = (tree_multiset*) xmalloc(sizeof(tree_multiset));   
    multiset->root = NULL;
    return multiset;
}

size_t multiset_count(tree_multiset* multiset, element_t key) {
    node* parent;
    node* found = find(multiset->root, key, &parent);
    
    if (found == NULL) {
        if (parent != NULL) {
            splay(multiset, parent);
        }

        return 0;
    } else {
        splay(multiset, found);

        return found->count;
    }
}

void multiset_insert(tree_multiset* multiset, element_t key) {
    if (multiset->root == NULL) {
        multiset->root = make_node(key);
        return;
    }

    node* parent;
    node* found = find(multiset->root, key, &parent);

    if (found == NULL) {
        node* created = make_node(key);

        if (key < parent->key) {
            parent->left = created;
        } else {
            parent->right = created;
        }

        created->parent = parent;

        propagate_diff_cool(created);
        splay(multiset, created);

    } else {
        splay(multiset, found);
        found->count++;

        maintain_diff_cool(found);
    }
}

void multiset_destroy(tree_multiset* multiset) {
    destroy_node(multiset->root);
    free(multiset);
}

size_t multiset_diff_cool(tree_multiset* multiset) {
    return multiset->root->diff_cool;
}
