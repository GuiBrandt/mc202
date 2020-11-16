/**
 * @file tree_multiset.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de multiset usando uma árvore multisplay [WDS06]

 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa08.html
 * 
 * [WDS06] C. C. Wang, J. Derryberry, and D. D. Sleator.
 *         O(log log n)-competitive dynamic binary search trees.
 *         SODA, pp 374–383, 2006.
 *         Disponível em https://www.cs.cmu.edu/~chengwen/paper/MST.pdf
 * 
 * Versão extendida do paper, também utilizada para referência, disponível em:
 * https://pdfs.semanticscholar.org/8006/044b0a69b9d1828711ce909f3201f49c7b06.pdf
 */

#include "tree_multiset.h"

#include <stdbool.h>
#include <assert.h>

/**
 * @brief Enumeração de cores de nós da árvore de referência. 
 */
typedef enum {
    RED, BLACK
} color;

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
    // imaginária com os nós da árvore multi-splay. 
    size_t ref_depth;

    // Valor de ref_depth mínimo na subárvore-splay do nó.
    size_t min_depth;

    // Determina se a conexão com o nó pai é tracejada (true) ou sólida
    // (false). Uma conexão tracejada significa que este nó é raíz de uma
    // árvore splay, ou ainda o nó filho não-preferido.
    bool is_splay_root : 1;

    // Cor do nó na árvore de referência
    color color : 1;
} node;

/**
 * @brief Mantém o valor auxiliar min_depth para um nó após alguma rotação ou
 *        alteração no filho preferido.
 * 
 * @param root nó envolvido na rotação.
 */
inline static void maintain_min_depth(node* root) {
    size_t min = root->ref_depth;

    if (
        root->left != NULL
        && !root->left->is_splay_root
        && root->left->min_depth < min
    ) {
        min = root->left->min_depth;        
    }

    if (
        root->right != NULL
        && !root->right->is_splay_root
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
inline static void rotate_left(node* p) {
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
    p->is_splay_root ^= q->is_splay_root;
    q->is_splay_root ^= p->is_splay_root;
    p->is_splay_root ^= q->is_splay_root;

    maintain_min_depth(p);
    maintain_min_depth(q);
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
inline static void rotate_right(node* p) {
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
    p->is_splay_root ^= q->is_splay_root;
    q->is_splay_root ^= p->is_splay_root;
    p->is_splay_root ^= q->is_splay_root;

    maintain_min_depth(p);
    maintain_min_depth(q);
}

/**
 * @brief Faz splay de um nó até que tenha um nó pai dado.
 * 
 * Se root != NULL, esta função assume que o nó dado está em uma das subárvores
 * splay do nó pai desejado. Caso contrário, a função tem comportamento
 * indeterminado.
 * 
 * Se root == NULL, esta função leva o nó dado até a raíz da árvore splay
 * correspondente.
 * 
 * @param subject nó sendo rotacionado.
 * @param root nó-pai desejado ao fim do splay.
 */
void splay(node* subject, node* root) {
    while (subject->parent != root && !subject->is_splay_root) {
        node* parent = subject->parent;
        node* grandparent = parent->is_splay_root ? NULL : parent->parent;

        assert(parent != NULL);

        if (parent->right == subject) {
            // Zag (Rotação para a esquerda)
            if (grandparent == root) {
                rotate_left(parent);
                return;
            }

            assert(grandparent != NULL);

            // Zag-Zag (em R)
            //
            //    P                      R
            //   / \                    / \ 
            //  x   Q                  Q   w
            //     / \       =>       / \ 
            //    y   R              P   z
            //       / \            / \ 
            //      z   w          x   y
            //
            //
            if (grandparent->right == parent) {
                rotate_left(grandparent);
                rotate_left(parent);

            // Zag-Zig (em P)
            //
            //        R 
            //       / \               P
            //      Q   w            /   \ 
            //     / \       =>     Q     R
            //    x   P            / \   / \ 
            //       / \          x   y z   w
            //      y   z
            //
            } else {
                assert(grandparent->left == parent);

                rotate_left(parent);
                rotate_right(grandparent);
            }

        } else {
            assert(parent->left == subject);

            // Zig (Rotação para a direita)
            if (grandparent == root) {
                rotate_right(parent);
                return;
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
}

/**
 * @brief Retorna o nó do predecessor (z) de um nó dado (y) seguindo o critério
 *        descrito na seção 3 de [WDS06].
 * 
 * @param y o nó dado.
 * 
 * @return o nó do predecessor.
 */
node* ref_left_parent(node* y) {
    node* current = y->left;
    int depth = y->ref_depth;

    if (
        current == NULL
        || current->is_splay_root
        || current->min_depth >= depth) {
        return NULL;
    }

    node* pred = NULL;

    while (current && current->min_depth < depth) {
        if (current->ref_depth < depth) {
            pred = current;
        }

        if (
            current->right == NULL
            || current->right->is_splay_root
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
 * Simétrico a ref_left_parent.
 * 
 * @param y o nó dado.
 * 
 * @return o nó do sucessor.
 */
node* ref_right_parent(node* y) {
    node* current = y->right;
    int depth = y->ref_depth;

    if (
        current == NULL
        || current->is_splay_root
        || current->min_depth >= depth
    ) {
        return NULL;
    }

    node* succ = NULL;

    while (current && current->min_depth < depth) {
        if (current->ref_depth < depth) {
            succ = current;
        }

        if (
            current->left == NULL
            || current->left->is_splay_root 
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

    // Predecessor (x) da subárvore esquerda de y na árvore de referência (L)
    node* x = ref_left_parent(y);

    // Sucessor (z) da subárvore direita de y na árvore de referência (R)
    node* z = ref_right_parent(y);

    // Subimos x e z até que sejam filhos de y, de forma que garantimos que a
    // direita de a é L e a subárvore esquerda de z é R.
    // Se x e/ou z não existem, então a subárvore esquerda de y deve ser L e
    // a subárvore direita de y deve ser R (seção 3.3.3 do paper extendido).

    node* l;
    if (x != NULL) {
        splay(x, y);
        l = x->right;
    } else {
        l = y->left;
    }

    node* r;
    if (z != NULL) {
        splay(z, y);
        r = z->left;
    } else {
        r = y->right;
    }

    // Invariante: durante uma troca, um dos nós trocados deve ser um caminho
    // preferido e outro não.
    assert(!l || !r || l->is_splay_root != r->is_splay_root);

    if (l != NULL) {
        l->is_splay_root ^= true;

        if (z) {
            maintain_min_depth(z);
        }
    }

    if (r != NULL) {
        r->is_splay_root ^= true;

        if (x) {
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
node* predecessor_on_splay(element_t key, node* splay_root) {
    node* current = splay_root;

    while (
        current->key >= key
        && current->left
        && !current->left->is_splay_root
    ) {
        current = current->left;
    }

    if (current->key >= key) {
        return NULL;
    }

    while (current->right && !current->right->is_splay_root) {
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
node* successor_on_splay(element_t key, node* splay_root) {
    node* current = splay_root;

    while (
        current->key <= key
        && current->right
        && !current->right->is_splay_root
    ) {
        current = current->right;
    }

    if (current->key <= key) {
        return NULL;
    }

    while (current->left && !current->left->is_splay_root) {
        current = current->left;
    }

    return current;    
}

/**
 * @brief Encontra o nó com menor profundidade na árvore de referência em uma
 *        árvore multi-splay com raíz dada.
 * 
 * @param root a raíz (!= NULL).
 * 
 * @return o nó com menor profundidade.
 */
node* ref_topmost(node* root) {
    assert(root != NULL);

    if (root->min_depth == root->ref_depth) {
        return root;
    }
    
    if (
        root->left != NULL &&
        (root->right == NULL || root->left->min_depth < root->right->min_depth)
    ) {
        return ref_topmost(root->left);
    } else {
        assert(root->right != NULL);
        assert(root->right->min_depth < root->ref_depth);
        
        return ref_topmost(root->right);
    }
}

/**
 * @brief Encontra o nó do filho esquerdo de um nó na árvore de referência.
 * 
 * Descrito na seção 5.2.2 do paper extendido.
 * 
 * @param v nó dado (!= NULL).
 * 
 * @return o nó do filho esquerdo na árvore de referência.
 */
node* ref_left_child(node* v) {
    assert(v != NULL);
    switch_preferred(v);
    
    node* t_l = ref_left_parent(v) == NULL ? v->left : v->left->right;
    
    node* l = NULL;

    if (t_l != NULL) {
        l = ref_topmost(t_l);
        switch_preferred(l);
        switch_preferred(l);
    }

    switch_preferred(v);

    return l;
}

/**
 * @brief Encontra o nó do filho direito de um nó na árvore de referência.
 * 
 * Descrito na seção 5.2.2 do paper extendido.
 * 
 * @param v nó dado (!= NULL).
 * 
 * @return o nó do filho direito na árvore de referência.
 */
node* ref_right_child(node* v) {
    assert(v != NULL);
    switch_preferred(v);
    
    node* t_r = ref_left_parent(v) == NULL ? v->right : v->right->left;
    
    node* r = NULL;

    if (t_r != NULL) {
        r = ref_topmost(t_r);
        switch_preferred(r);
        switch_preferred(r);
    }

    switch_preferred(v);

    return r;
}

/**
 * @brief Encontra a raíz da árvore splay de um nó dado.
 * 
 * @param v nó dado (!= NULL).
 * 
 * @return o nó raíz da árvore splay correspondente.
 */
node* splay_root(node* v) {
    assert(v != NULL);

    node* r = v;

    while (!r->is_splay_root) {
        r = r->parent;
    }

    return r;
}

/**
 * @brief Encontra o pai esquerdo de um nó na árvore (ignorando os caminhos
 *        preferidos).
 * 
 * @param root nó raíz de uma árvore splay.
 * 
 * @return o nó do pai esquerdo do nó dado.
 */
node* left_parent(node* root) {
    assert(root != NULL);

    node* current = root;

    while (current->parent != NULL) {
        if (current->parent->right == current) {
            return current->parent;
        }
    
        current = current->parent;
    }

    return NULL;
}

/**
 * @brief Encontra o pai direito de um nó na árvore (ignorando os caminhos
 *        preferidos).
 * 
 * @param root nó raíz de uma árvore splay.
 * 
 * @return o nó do pai direito do nó dado.
 */
node* right_parent(node* root) {
    node* current = root;

    while (current->parent != NULL) {
        if (current->parent->left == current) {
            return current->parent;
        }
    
        current = current->parent;
    }

    return NULL;
}

/**
 * @brief Encontra o nó do pai de um nó na árvore de referência.
 * 
 * Descrito na seção 5.2.2 do paper extendido.
 * 
 * @param v nó dado (!= NULL).
 * 
 * @return o nó do pai na árvore de referência.
 */
node* ref_parent(node* v) {
    switch_preferred(v);
    switch_preferred(v);

    size_t parent_depth = v->ref_depth - 1;

    if (
        v->left != NULL
        && !v->left->is_splay_root
        && v->left->ref_depth == parent_depth
    ) {
        return v->left;
    }
    
    if (
        v->right != NULL
        && !v->right->is_splay_root
        && v->right->ref_depth == parent_depth
    ) {
        return v->right;
    }

    node* r = splay_root(v);

    node* l_p = left_parent(r);
    if (l_p != NULL && l_p->ref_depth == parent_depth) {
        return l_p;
    }

    node* r_p = right_parent(r);
    if (r_p != NULL && r_p->ref_depth == parent_depth) {
        return r_p;
    }

    return NULL;
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
        backtrack != NULL && backtrack->parent != NULL;
        backtrack = backtrack->parent
    ) {
        // Queremos ajustar apenas as aretas não-preferidas
        if (!backtrack->is_splay_root) {
            continue;
        }

        node* v = predecessor_on_splay(found->key, backtrack->parent);
        node* w = successor_on_splay(found->key, backtrack->parent);

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
        if (root->left->is_splay_root) {
            printf(" [style=dashed];");
        }
        printf("\n");
    }

    if (root->right) {
        graphviz(root->right, id);
        printf("\"%d_%"PRIu64"\" -- \"%d_%"PRIu64"\"", id, root->key, id, root->right->key);
        if (root->right->is_splay_root) {
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
    a.is_splay_root = false;
    a.min_depth = 3;
    a.ref_depth = 3;

    b.parent = &e;
    b.left = &a;
    b.right = &c;
    b.is_splay_root = false;
    b.min_depth = 2;
    b.ref_depth = 2;
    
    c.parent = &b;
    c.right = &d;
    c.is_splay_root = true;
    c.min_depth = 3;
    c.ref_depth = 3;

    d.parent = &c;
    d.is_splay_root = false;
    d.min_depth = 4;
    d.ref_depth = 4;

    e.parent = NULL;
    e.left = &b;
    e.right = &h;
    e.is_splay_root = true;
    e.min_depth = 1;
    e.ref_depth = 1;
    
    f.parent = &h;
    f.right = &g;
    f.is_splay_root = false;
    f.min_depth = 3;
    f.ref_depth = 3;

    g.parent = &f;
    g.is_splay_root = false;
    g.min_depth = 4;
    g.ref_depth = 4;
    
    h.parent = &e;
    h.left = &f;
    h.right = &i;
    h.is_splay_root = true;
    h.min_depth = 2;
    h.ref_depth = 2;
    
    i.parent = &h;
    i.right = &j;
    i.is_splay_root = true;
    i.min_depth = 3;
    i.ref_depth = 3;
    
    j.parent = &i;
    j.is_splay_root = false;
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

    node* l = ref_left_child(&e);
    printf("%"PRIu64"\n", l->key);

    node* r = ref_right_child(&e);
    printf("%"PRIu64"\n", r->key);

    printf("%"PRIu64"\n", ref_parent(ref_right_child(&e))->key);
    printf("%"PRIu64"\n", ref_parent(ref_left_child(&e))->key);
    printf("%p\n", ref_parent(&e));

    return 0;
}
