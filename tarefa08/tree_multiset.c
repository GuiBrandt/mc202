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
 * @brief Mantém o valor auxiliar min_depth para um nó após alguma rotação ou
 *        alteração no filho preferido.
 * 
 * @param root nó envolvido na rotação.
 */
inline static void maintain_min_depth(node* root) {
    size_t min = root->reference_depth;

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
    while (subject->parent != root) {
        node* parent = subject->parent;
        node* grandparent = parent->parent;

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
    node* current = y;
    int depth = y->reference_depth;

    // Encontra o primeiro nó igual a y ou acima na mesma árvore splay onde
    // existe algum nó com profundidade menor que y na árvore referência.
    while (current->min_depth >= depth) {
        node* parent = current->parent;

        // Se o pai é direito ou se o nó atual é raíz da árvore splay,
        // não existe nó com profundidade < y.reference_depth na árvore.
        if (!parent || parent->left == current || current->is_dashed) {
            return NULL;
        }

        current = current->parent;
    }

    if (current == y) {
        current = current->left;
    }

    if (current == NULL) {
        return NULL;
    }

    // Buscamos o nó de maior valor possível que ainda tenha profundidade
    // menor que y.
    // Para isso, basta um percurso para a direita enquanto a profundidade
    // mínima da árvore splay com raíz no nó for menor que a profundidade
    // desejada.
    // Isso é verdadeiro por construção, uma vez que garantimos que o valor de
    // min_depth dos nós filhos deve ser sempre maior ou igual ao valor do nó
    // pai (por definição de mínimo), e os valores na subárvore esquerda devem
    // ser sempre menores que o valor do pai (propriedade da ABB).
    while (
        current->right
        && !current->right->is_dashed
        && current->right->min_depth < depth
    ) {
        current = current->right;
    }

    return current;
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
    node* current = y;
    int depth = y->reference_depth;

    while (current->min_depth >= depth) {
        node* parent = current->parent;

        if (!parent || parent->right == current || current->is_dashed) {
            return NULL;
        }

        current = current->parent;
    }

    if (current == y) {
        current = current->right;
    }

    if (current == NULL) {
        return NULL;
    }

    while (
        current->left
        && !current->left->is_dashed
        && current->left->min_depth < depth
    ) {
        current = current->left;
    }

    return current;
}

/**
 * @brief Troca o filho preferido de um nó.
 * 
 * @param y nó para o qual trocar o filho preferido.
 */
void switch_preferred(node* y) {
    if (y->left) {
        y->left->is_dashed = false;
    }

    if (y->right) {
        y->right->is_dashed = false;
    }

    node* z = ref_predecessor(y);
    node* x = ref_successor(y);

    splay(y, NULL);

    if (z) {
        splay(z, y);
    }

    if (x) {
        splay(x, y);
    }

    if (x != NULL && z != NULL) {
        if (z->right) {
            z->right->is_dashed = !z->right->is_dashed;
            maintain_min_depth(z);
        }

        if (x->left) {
            x->left->is_dashed = !x->left->is_dashed;
            maintain_min_depth(x);
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

// Complexidade: O(log^2 n) pior caso       [WDS06, teorema 4.1]
//               O(log log n)-competitiva   [WDS06, teorema 4.2]
//               O(log n) amortizado        [WDS06, teorema 4.3]
//
// Em particular, se a sequência de acessos for sequencial, o tempo gasto ao
// todo é O(n).
size_t multiset_count(tree_multiset* multiset, element_t elem) {

    // Busca em ABB convencional
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

    // Procedimento de correção de caminho preferido (descrito em [WDS06],
    // seção 3).
    node* backtrack = current;
    while (backtrack && backtrack->parent != NULL) {
        if (backtrack->is_dashed) {
            node* v = splay_predecessor(elem, backtrack->parent);
            node* w = splay_successor(elem, backtrack->parent);
            
            if (
                v != NULL
                && (w == NULL || v->reference_depth < w->reference_depth)
            ) {
                switch_preferred(v);
                
            } else if (w) {
                switch_preferred(w);
            }
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

#include <time.h>
#include <stdlib.h>

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
    a.reference_depth = 3;

    b.parent = &e;
    b.left = &a;
    b.right = &c;
    b.is_dashed = true;
    b.min_depth = 2;
    b.reference_depth = 2;
    
    c.parent = &b;
    c.right = &d;
    c.is_dashed = true;
    c.min_depth = 3;
    c.reference_depth = 3;

    d.parent = &c;
    d.is_dashed = true;
    d.min_depth = 4;
    d.reference_depth = 4;

    e.parent = NULL;
    e.left = &b;
    e.right = &h;
    e.is_dashed = false;
    e.min_depth = 1;
    e.reference_depth = 1;
    
    f.parent = &h;
    f.right = &g;
    f.is_dashed = false;
    f.min_depth = 3;
    f.reference_depth = 3;

    g.parent = &f;
    g.is_dashed = true;
    g.min_depth = 4;
    g.reference_depth = 4;
    
    h.parent = &e;
    h.left = &f;
    h.right = &i;
    h.is_dashed = true;
    h.min_depth = 2;
    h.reference_depth = 2;
    
    i.parent = &h;
    i.right = &j;
    i.is_dashed = true;
    i.min_depth = 3;
    i.reference_depth = 3;
    
    j.parent = &i;
    j.is_dashed = true;
    j.min_depth = 4;
    j.reference_depth = 4;

    tree_multiset s = { &e };

    printtree(s.root);
    printf("\n");

    srand(time(NULL));

    for (int i = 0; i < 10000; i++) {
        multiset_count(&s, rand() % 10 + 1);
        printtree(s.root);
        printf("\n");
    }

    return 0;
}
