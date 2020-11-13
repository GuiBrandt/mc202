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
typedef struct tree_multiset_node {
    // Chave do nó
    element_t key;

    // Número de repetições no conjunto
    size_t count;

    // Filho esquerdo
    struct tree_multiset_node* left;

    // Filho direito
    struct tree_multiset_node* right;

    // Nó pai (NULL para a raíz)
    struct tree_multiset_node* parent;

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

/**
 * @brief Retorna o nó mínimo (mais à esquerda) dado um nó raíz.
 * 
 * @param root o nó raíz.
 * 
 * @return o nó mínimo na árvore.
 */
node* minimum_with_depth(node* root, int max_depth) {
    node* current = root;
    node* best = NULL;

    while (current->left) {
        current = current->left;

        if (current->reference_depth < max_depth) {
            best = current;
        }
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
node* maximum_with_depth(node* root, int max_depth) {
    node* current = root;
    node* best = NULL;

    while (current->right) {
        current = current->right;

        if (current->reference_depth < max_depth) {
            best = current;
        }
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
node* predecessor_with_depth(node* root, int max_depth) {
    if (root->left == NULL) {
        return NULL;
    }

    return maximum_with_depth(root->left, max_depth);
}

/**
 * @brief Retorna o nó do sucessor de um nó dado.
 * 
 * @param root o nó dado.
 * 
 * @return o nó do sucessor.
 */
node* successor_with_depth(node* root, int max_depth) {
    if (root->right == NULL) {
        return NULL;
    }

    return minimum_with_depth(root->right, max_depth);
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
inline static node* rotate_left(node* p) {
    assert(p->right != NULL);

    node* o = p->parent;
    node* q = p->right;
    node* y = q->left;
    
    q->left = p;
    p->right = y;

    if (o) {
        if (o->right == p) {
            o->right = q;
        } else {
            o->left = q;
        }
    }

    if (y) {
        y->parent = p;
    }

    p->parent = q;
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
 * Onde P é o nó sendo rotacionado. Os nós em maiúscula são obrigatórios (i.e.
 * não podem ser NULL).
 * 
 * @param root o nó sendo rotacionado.
 */
inline static node* rotate_right(node* p) {
    assert(p->left != NULL);

    node* o = p->parent;
    node* q = p->left;
    node* y = q->right;

    q->right = p;
    p->left = y;

    if (o) {
        if (o->right == p) {
            o->right = q;
        } else {
            o->left = q;
        }
    }

    p->parent = q;
    q->parent = o;

    if (y) {
        y->parent = p;
    }

    return q;
}

/**
 * @brief Faz splay de um nó até que tenha um nó pai dado.
 * 
 * Se root != NULL, esta função assume que o nó dado está em uma das subárvores
 * do nó pai desejado. Caso contrário, a função tem comportamento indeterminado.
 * 
 * Se root == NULL, esta função leva o nó dado até a raíz da árvore.
 * 
 * @param subject nó sendo rotacionado.
 * @param root nó-pai desejado ao fim do splay.
 */
void splay(node* subject, node* root) {
    while (subject->parent != root) {
        node* parent = subject->parent;
        node* grandparent = parent->parent;

        assert(parent != NULL);

        if (parent->right == subject) {
            // Lida com o caso zag-zag, onde o avô deve ser rotacionado antes
            // do pai
            if (grandparent != root && grandparent->right == parent) {
                rotate_left(grandparent);
            }

            rotate_left(parent);

        } else {
            assert(parent->left == subject);

            // Lida com o caso zig-zig, onde o avô deve ser rotacionado antes
            // do pai
            if (grandparent != root && grandparent->left == parent) {
                rotate_right(grandparent);
            }
            
            rotate_right(parent);
        }
    }
}

/**
 * @brief Troca o filho preferido de um nó.
 * 
 * @param y nó para o qual trocar o filho preferido.
 */
void switch_preferred(node* y) {
    node* z = predecessor_with_depth(y, y->reference_depth);
    node* x = successor_with_depth(y, y->reference_depth);

    splay(y, NULL);

    if (z) {
        splay(z, y);

        if (z->right) {
            z->right->is_dashed = !z->right->is_dashed;
        }
    }

    if (x) {
        splay(x, y);

        if (x->left) {
            x->left->is_dashed = !x->left->is_dashed;
        }
    }
}

struct tree_multiset {
    node* root;
};

size_t multiset_count(tree_multiset* multiset, element_t elem) {
    node* current = multiset->root;

    while (current && current->key != elem) {
        if (elem < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    if (current == NULL) {
        return 0;
    }

    node* backtrack = current;
    while (backtrack && backtrack->parent != NULL) {
        if (backtrack->is_dashed) {
            switch_preferred(backtrack->parent);
        }

        backtrack = backtrack->parent;
    }

    switch_preferred(current);

    multiset->root = current;

    return current->count;
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
    c.reference_depth = 1;
    c.is_dashed = true;

    b.left = &a;
    b.parent = &c;
    b.reference_depth = 2;
    b.is_dashed = false;

    a.parent = &b;
    a.reference_depth = 3;
    a.is_dashed = true;

    d.right = &e;
    d.parent = &c;
    d.reference_depth = 2;

    e.parent = &d;
    e.is_dashed = true;
    c.reference_depth = 3;

    tree_multiset s = { &c };

    printtree(s.root);
    printf("\n");

    multiset_count(&s, 3);
    printtree(s.root);
    printf("\n");

    multiset_count(&s, 2);
    printtree(s.root);
    printf("\n");

    multiset_count(&s, 4);
    printtree(s.root);
    printf("\n");

    multiset_count(&s, 1);
    printtree(s.root);
    printf("\n");

    multiset_count(&s, 5);
    printtree(s.root);
    printf("\n");

    return 0;
}
