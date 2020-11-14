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
    size_t ref_depth;

    // Valor de profundidade mínimo na subárvore-splay do nó.
    size_t min_depth;

    // Determina se a conexão com o nó pai é tracejada (true) ou sólida
    // (false). Uma conexão tracejada significa que este nó é raíz de uma
    // árvore splay, ou ainda o nó filho não-preferido.
    bool is_dashed : 1;
} node;

/**
 * @brief Mantém o valor auxiliar min_depth para um nó após alguma rotação ou
 *        alteração no filho preferido.
 * 
 * @param root nó envolvido na rotação.
 */
inline static void maintain_min_depth(node* root) {
    size_t min = root->ref_depth;

    if (root->left && !root->left->is_dashed && root->left->min_depth < min) {
        min = root->left->min_depth;        
    }

    if (
        root->right
        && !root->right->is_dashed
        && root->right->min_depth < min
    ) {
        min = root->right->min_depth;
    }

    root->min_depth = min;
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

    // Troca o tipo de aresta para p e q
    p->is_dashed ^= q->is_dashed;
    q->is_dashed ^= p->is_dashed;
    p->is_dashed ^= q->is_dashed;

    maintain_min_depth(p);
    maintain_min_depth(q);

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

    // Troca o tipo de aresta para p e q
    p->is_dashed ^= q->is_dashed;
    q->is_dashed ^= p->is_dashed;
    p->is_dashed ^= q->is_dashed;

    maintain_min_depth(p);
    maintain_min_depth(q);

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
    while (subject->parent != root && !subject->is_dashed) {
        node* parent = subject->parent;
        node* grandparent = parent->is_dashed ? NULL : parent->parent;

        assert(parent != NULL);

        if (parent->right == subject) {
            // Zag
            if (grandparent == root) {
                rotate_left(parent);
                return;
            }

            assert(grandparent != NULL);

            // Zag-Zag
            if (grandparent->right == parent) {
                rotate_left(grandparent);
                rotate_left(parent);

            // Zag-Zig
            } else {
                assert(grandparent->left == parent);

                rotate_left(parent);
                rotate_right(grandparent);
            }

        } else {
            assert(parent->left == subject);

            // Zig
            if (grandparent == root) {
                rotate_right(parent);
                return;
            }

            assert(grandparent != NULL);

            // Zig-Zag
            if (grandparent->right == parent) {
                rotate_right(parent);
                rotate_left(grandparent);

            // Zig-Zig
            } else {
                assert(grandparent->left == parent);

                rotate_right(grandparent);
                rotate_right(parent);
            }
        }
    }
}

/**
 * @brief Retorna o nó do predecessor (z) de um nó dado (y) seguindo o critério
 *        descrito na seção 3 de [WDS06].
 * 
 * @param y o nó dado.
 * 
 * @return o nó do predecessor.
 */
node* ref_predecessor(node* y) {
    node* current = y->left;
    int depth = y->ref_depth;

    if (!current || current->is_dashed || current->min_depth >= depth) {
        return NULL;
    }

    node* pred = NULL;

    while (current && current->min_depth < depth) {
        if (current->ref_depth < depth) {
            pred = current;
        }

        if (
            current->right == NULL
            || current->right->is_dashed
            || current->right->min_depth >= depth
        ) {
            if (pred == NULL) {
                current = current->left;
            } else {
                break;
            }
        } else {
            current = current->right;
        }
    }

    return pred;
}

/**
 * @brief Retorna o nó do sucessor (x) de um nó dado (y) seguindo o critério
 *        descrito na seção 3 de [WDS06].
 * 
 * Simétrico a ref_predecessor.
 * 
 * @param y o nó dado.
 * 
 * @return o nó do sucessor.
 */
node* ref_successor(node* y) {
    node* current = y->right;
    int depth = y->ref_depth;

    if (!current || current->is_dashed || current->min_depth >= depth) {
        return NULL;
    }

    node* succ = NULL;

    while (current && current->min_depth < depth) {
        if (current->ref_depth < depth) {
            succ = current;
        }

        if (
            current->left == NULL
            || current->left->is_dashed 
            || current->left->min_depth >= depth
        ) {
            if (succ == NULL) {
                current = current->right;
            } else {
                break;
            }
        } else {
            current = current->left;
        }
    }

    return succ;
}

/**
 * @brief Troca o filho preferido de um nó na árvore de referência.
 * 
 * Esse algoritmo é descrito na seção 3 de [WDS06].
 * 
 * @param y nó para o qual trocar o filho preferido.
 */
void switch_preferred(node* y) {
    // Fazemos splay de y até a raíz, para garantir que x e z são descendentes
    // de y.
    splay(y, NULL);

    // Predecessor da subárvore esquerda de y na árvore de referência (L)
    node* z = ref_predecessor(y);

    // Sucessor da subárvore direita de y na árvore de referência (R)
    node* x = ref_successor(y);

    // Subimos z e x até que sejam filhos de y, de forma que garantimos que a
    // direita de z é L e a subárvore esquerda de x é R.
    node* l;
    if (z != NULL) {
        splay(z, y);
        l = z->right;
    } else {
        l = y->left;
    }

    node* r;
    if (x != NULL) {
        splay(x, y);
        r = x->left;
    } else {
        r = y->right;
    }

    assert(!l || !r || l->is_dashed != r->is_dashed);

    if (l != NULL) {
        l->is_dashed ^= true;

        if (x) {
            maintain_min_depth(x);
        }
    }

    if (r != NULL) {
        r->is_dashed ^= true;

        if (z) {
            maintain_min_depth(z);
        }
    }
}

/**
 * @brief Retorna o nó do predecessor de um valor dado em uma árvore splay
 *        dada.
 * 
 * @param key chave do valor buscado.
 * @param splay_root raíz da árvore splay.
 * 
 * @return o nó contendo o predecessor do valor na árvore, ou NULL caso o valor
 *         seja mínimo.
 */
node* splay_predecessor(element_t key, node* splay_root) {
    node* current = splay_root;

    while (
        current->key >= key
        && current->left
        && !current->left->is_dashed
    ) {
        current = current->left;
    }

    if (current->key >= key) {
        return NULL;
    }

    while (current->right && !current->right->is_dashed) {
        current = current->right;
    }

    return current;
}

/**
 * @brief Retorna o nó do sucessor de um valor dado em uma árvore splay dada.
 * 
 * @param key chave do valor buscado.
 * @param splay_root raíz da árvore splay.
 * 
 * @return o nó contendo o sucessor do valor na árvore, ou NULL caso o valor
 *         seja mínimo.
 */
node* splay_successor(element_t key, node* splay_root) {
    node* current = splay_root;

    while (
        current->key <= key
        && current->right
        && !current->right->is_dashed
    ) {
        current = current->right;
    }

    if (current->key <= key) {
        return NULL;
    }

    while (current->left && !current->left->is_dashed) {
        current = current->left;
    }

    return current;    
}

struct tree_multiset {
    node* root;
};

/**
 * @brief Troca o caminho preferido para um nó mantendo o ponteiro da raíz de
 *        um conjunto atualizado. 
 * 
 * @param multiset o conjunto.
 * @param y o nó cujo caminho preferido será trocado.
 */
void switch_and_maintain_root(tree_multiset* multiset, node* y) {
    switch_preferred(y);

    if (y->parent == NULL) {
        multiset->root = y;
    }
}

/**
 * @brief Procedimento de correção de caminho preferido.
 * 
 * Descrito em [WDS06], seção 3.
 * 
 * @param multiset o conjunto.
 * @param found o nó com o valor encontrado.
 */
void multi_splay(tree_multiset* multiset, node* found) {
    for (
        node* backtrack = found;
        backtrack && backtrack->parent != NULL;
        backtrack = backtrack->parent
    ) {
        if (!backtrack->is_dashed) {
            continue;
        }

        node* v = splay_predecessor(found->key, backtrack->parent);
        node* w = splay_successor(found->key, backtrack->parent);

        if (v != NULL && (w == NULL || v->ref_depth < w->ref_depth)) {
            switch_and_maintain_root(multiset, v);
        } else if (w != NULL) {
            switch_and_maintain_root(multiset, w);
        }
    }

    switch_and_maintain_root(multiset, found);
}

// Complexidade: O(log^2 n) pior caso       [WDS06, teorema 4.1]
//               O(log log n)-competitiva   [WDS06, teorema 4.2]
//               O(log n) amortizado        [WDS06, teorema 4.3]
//
// Em particular, se a sequência de acessos for sequencial, o tempo gasto ao
// todo é O(n).
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

    multi_splay(multiset, current);

    return current->count;
}

#include <stdio.h>
#include <inttypes.h>

#include <time.h>
#include <stdlib.h>

void graphviz(node* root, int id) {
    printf("\"%d_%"PRIu64"\" [label=\"%"PRIu64"\"];\n", id, root->key, root->key);

    if (root->left) {
        graphviz(root->left, id);
        printf("\"%d_%"PRIu64"\" -- \"%d_%"PRIu64"\"", id, root->key, id, root->left->key);
        if (root->left->is_dashed) {
            printf(" [style=dashed];");
        }
        printf("\n");
    }

    if (root->right) {
        graphviz(root->right, id);
        printf("\"%d_%"PRIu64"\" -- \"%d_%"PRIu64"\"", id, root->key, id, root->right->key);
        if (root->right->is_dashed) {
            printf(" [style=dashed];");
        }
        printf("\n");
    }
}

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
    node f = { 6, 1, 0 };
    node g = { 7, 1, 0 };
    node h = { 8, 1, 0 };
    node i = { 9, 1, 0 };
    node j = { 10, 1, 0 };

    a.parent = &b;
    a.is_dashed = false;
    a.min_depth = 3;
    a.ref_depth = 3;

    b.parent = &e;
    b.left = &a;
    b.right = &c;
    b.is_dashed = false;
    b.min_depth = 2;
    b.ref_depth = 2;
    
    c.parent = &b;
    c.right = &d;
    c.is_dashed = true;
    c.min_depth = 3;
    c.ref_depth = 3;

    d.parent = &c;
    d.is_dashed = false;
    d.min_depth = 4;
    d.ref_depth = 4;

    e.parent = NULL;
    e.left = &b;
    e.right = &h;
    e.is_dashed = true;
    e.min_depth = 1;
    e.ref_depth = 1;
    
    f.parent = &h;
    f.right = &g;
    f.is_dashed = false;
    f.min_depth = 3;
    f.ref_depth = 3;

    g.parent = &f;
    g.is_dashed = false;
    g.min_depth = 4;
    g.ref_depth = 4;
    
    h.parent = &e;
    h.left = &f;
    h.right = &i;
    h.is_dashed = true;
    h.min_depth = 2;
    h.ref_depth = 2;
    
    i.parent = &h;
    i.right = &j;
    i.is_dashed = true;
    i.min_depth = 3;
    i.ref_depth = 3;
    
    j.parent = &i;
    j.is_dashed = false;
    j.min_depth = 4;
    j.ref_depth = 4;

    tree_multiset s = { &e };

    printf("graph {\n");

    printf("subgraph cluster_ref {\n");
    printf("label=\"Reference\";");
    graphviz(s.root, 0);
    printf("}\n");

    srand(time(NULL));

    for (int i = 0; i < 100; i++) {
        multiset_count(&s, rand() % 10 + 1);
        printf("subgraph cluster_%d {\n", i + 1);
        graphviz(s.root, i + 1);
        printf("}\n");
    }

    printf("}\n");

    return 0;
}
