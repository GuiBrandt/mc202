/**
 * @file tree_multiset.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de multiset usando uma árvore multisplay [WDS06]
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa08.html
 * 
 * [WDS06] C. C. Wang, J. Derryberry, and D. D. Sleator.
 *         O(log log n)-competitive dynamic binary search trees.
 *         SODA, pp 374–383, 2006.
 *         Disponível em https://www.cs.cmu.edu/~chengwen/paper/MST.pdf
 */

#include "tree_multiset.h"

#include <stdbool.h>
#include <assert.h>

/**
 * @brief Estrutura de nó da árvore multi-splay. 
 */
typedef struct tree_multiset {
    // Chave do nó
    element_t key;

    // Número de repetições no conjunto
    size_t count;

    // Filho esquerdo
    struct tree_multiset* left;

    // Filho direito
    struct tree_multiset* right;

    // Nó pai (NULL para a raíz)
    struct tree_multiset* parent;

    // Profundidade do nó na árvore de referência.
    // A árvore de referência é uma árvore binária de busca balanceada
    // imaginária com os nós da árvore multisplay. 
    size_t reference_depth;

    // Valor de profundidade mínimo na subárvore-splay do nó.
    size_t min_depth;

    // Determina se a conexão com o nó pai é tracejada (true) ou sólida
    // (false). Uma conexão tracejada significa que este nó é raíz de uma
    // árvore splay, ou ainda o nó filho não-preferido.
    bool is_dashed;
} node;

node* minimum(node* root);
node* maximum(node* root);

node* predecessor(node* root);
node* successor(node* root);

node* rotate_left(node* root);
node* rotate_right(node* root);

void splay(node* subject, node* root);

void switch_preferred(node* root);

/**
 * @brief Retorna o nó mínimo (mais à esquerda) dado um nó raíz.
 * 
 * @param root o nó raíz.
 * 
 * @return o nó mínimo na árvore.
 */
node* minimum(node* root) {
    node* current = root;
    while (current->left) {
        current = current->left;
    }

    return current;
}

/**
 * @brief Retorna o nó máximo (mais à direita) dado um nó raíz.
 * 
 * @param root o nó raíz.
 * 
 * @return o nó máximo na árvore.
 */
node* maximum(node* root) {
    node* current = root;
    while (current->right) {
        current = current->right;
    }

    return current;
}

/**
 * @brief Retorna o nó do predecessor de um nó dado.
 * 
 * @param root o nó dado.
 * 
 * @return o nó do predecessor.
 */
node* predecessor(node* root) {
    if (root->left == NULL) {
        return NULL;
    }

    return maximum(root->left);
}

/**
 * @brief Retorna o nó do sucessor de um nó dado.
 * 
 * @param root o nó dado.
 * 
 * @return o nó do sucessor.
 */
node* successor(node* root) {
    if (root->right == NULL) {
        return NULL;
    }

    return minimum(root->right);
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
 * Onde P é o nó sendo rotacionado.
 * 
 * @param root o nó sendo rotacionado.
 */
node* rotate_left(node* p) {
    assert(p->right != NULL);

    node* o = p->parent;
    node* q = p->right;
    node* y = q->left;
    
    q->left = p;
    p->parent = q;

    p->right = y;
    if (y) {
        y->parent = p;
    }

    if (o) {
        if (p == o->right) {
            o->right = q;
        } else {
            o->left = q;
        }
    }

    q->parent = o;

    return q;
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
 * Onde P é o nó sendo rotacionado.
 * 
 * @param root o nó sendo rotacionado.
 */
node* rotate_right(node* p) {
    assert(p->left != NULL);

    node* o = p->parent;
    node* q = p->left;
    node* y = q->right;

    q->right = p;
    p->parent = q;

    p->left = y;
    if (y) {
        y->parent = p;
    }

    if (o) {
        if (p == o->right) {
            o->right = q;
        } else {
            o->left = q;
        }
    }

    q->parent = o;

    return q;
}

#include <stdio.h>
#include <inttypes.h>

void printtree(node* root) {
    printf("(");
    
    if (root->left)
        printtree(root->left);

    printf(" %"PRIu64" ", root->key);
    
    if (root->right)
        printtree(root->right);

    printf(")");
}

int main() {
    node a = { 1, 1, 0 };
    node b = { 2, 1, 0 };
    node c = { 3, 1, 0 };
    node d = { 4, 1, 0 };
    node e = { 5, 1, 0 };

    c.left = &b;
    c.right = &d;

    b.left = &a;
    b.parent = &c;

    a.parent = &b;

    d.right = &e;
    d.parent = &c;

    e.parent = &d;

    printtree(&c);
    printf("\n");

    rotate_left(&d);

    printtree(&c);
    printf("\n");

    rotate_left(&c);

    printtree(&e);
    printf("\n");

    rotate_right(&e);

    printtree(&c);
    printf("\n");

    return 0;
}
