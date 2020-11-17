/**
 * @file tree_multiset.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de multiset usando uma árvore multisplay [WDS06]

 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa08.html
 * 
 * ## Referências
 * 
 * [WDS06] C. C. Wang, J. Derryberry, e D. D. Sleator.
 *         O(log log n)-competitive dynamic binary search trees.
 *         SODA, pp. 374–383, 2006.
 *         Disponível em https://www.cs.cmu.edu/~chengwen/paper/MST.pdf
 *         Versão extendida do paper, também utilizada para referência,
 *         disponível em https://pdfs.semanticscholar.org/8006/044b0a69b9d1828711ce909f3201f49c7b06.pdf.
 * 
 * [ST85] D. D. Sleator e R. E. Tarjan.
 *        Self-Adjusting Binary Search Trees.
 *        Journal of the Association for Computing Machinery, Vol. 32, No. 3,
 *        pp. 652-686, 1985. 
 *        Disponível em https://www.cs.cmu.edu/~sleator/papers/self-adjusting.pdf.
 */

#include "tree_multiset.h"

#include <stdbool.h>
#include <assert.h>

#include <stdio.h>
#include <inttypes.h>

#include <time.h>
#include <stdlib.h>

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

    // Diferença de profundidade do nó na árvore de referência em relação ao
    // pai na árvore multi-splay.
    int delta_ref_depth;

    // Diferença de valor de ref_depth mínimo na subárvore-splay do nó em
    // relação ao pai.
    int delta_min_depth;

    // Determina se a conexão com o nó pai é tracejada (true) ou sólida
    // (false). Uma conexão tracejada significa que este nó é raíz de uma
    // árvore splay, ou ainda o nó filho não-preferido.
    bool is_splay_root : 1;

    // Cor do nó na árvore de referência
    color color : 1;
} node;


void graphviz(node* root, int id) {
    printf("\"%d_%"PRIu64"\" [label=\"%"PRIu64" | %d | %d\"];\n", id, root->key, root->key, root->delta_ref_depth, root->delta_min_depth);

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

/**
 * @brief Corrige o valor auxiliar delta_min_depth para um nó após alguma
 *        rotação ou alteração no filho preferido.
 * 
 * Descrito em [ST85], seção 6.
 * 
 * @param root nó envolvido na rotação.
 */
inline static void maintain_min_depth(node* root) {
    int max = 0;

    if (root->left != NULL && !root->left->is_splay_root) {
        int d = root->left->delta_min_depth - root->left->delta_ref_depth;
        
        if (d > max) {
            max = d;
        }
    }

    if (root->right != NULL && !root->right->is_splay_root) {
        int d = root->right->delta_min_depth - root->right->delta_ref_depth;

        if (d > max) {
            max = d;
        }
    }

    root->delta_min_depth = max;
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
    assert(p != NULL);
    assert(p->right != NULL);

    node* o = p->parent;
    node* q = p->right;
    node* y = q->left;

    assert(y == NULL || !q->is_splay_root || y->is_splay_root);
    
    q->left = p;
    p->right = y;

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

    // Troca o tipo de aresta para p e q
    p->is_splay_root ^= q->is_splay_root;
    q->is_splay_root ^= p->is_splay_root;
    p->is_splay_root ^= q->is_splay_root;

    // Corrige os valores de delta_ref_depth
    int delta_q = q->delta_ref_depth;
    q->delta_ref_depth += p->delta_ref_depth;
    p->delta_ref_depth = -delta_q;

    if (y != NULL) {
        y->delta_ref_depth += delta_q;
    }

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
    assert(p != NULL);
    assert(p->left != NULL);

    node* o = p->parent;
    node* q = p->left;
    node* y = q->right;

    assert(y == NULL || !q->is_splay_root || y->is_splay_root);

    q->right = p;
    p->left = y;

    if (o != NULL) {
        if (o->right == p) {
            o->right = q;
        } else {
            o->left = q;
        }
    }

    p->parent = q;
    q->parent = o;

    if (y != NULL) {
        y->parent = p;
    }

    // Troca o tipo de aresta para p e q
    p->is_splay_root ^= q->is_splay_root;
    q->is_splay_root ^= p->is_splay_root;
    p->is_splay_root ^= q->is_splay_root;

    // Corrige os valores de delta_ref_depth
    int delta_q = q->delta_ref_depth;
    q->delta_ref_depth += p->delta_ref_depth;
    p->delta_ref_depth = -delta_q;

    if (y != NULL) {
        y->delta_ref_depth += delta_q;
    }

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
 * @brief Retorna o nó do predecessor (x) de um nó dado (y) seguindo o critério
 *        descrito na seção 3 de [WDS06].
 * 
 * @param y o nó dado.
 * 
 * @return o nó do predecessor.
 */
node* ref_left_parent(node* y) {
    assert(y != NULL);

    node* current = y->left;

    // delta_ref_depth - delta_min_depth = min_depth relativo a y, se for maior
    // ou igual a 0, não existem nós com profundidade menor que y na subárvore,
    // i.e. não existe pai esquerdo.
    if (
        current == NULL
        || current->is_splay_root
        || current->delta_ref_depth - current->delta_min_depth >= 0
    ) {
        return NULL;
    }

    // Todos os cálculos com profundidade são feitos relativos à raíz da
    // subárvore esquerda.
    // A variável depth acumula o valor de profundidade relativa durante o
    // percurso, de forma que depth - nó->delta_min_depth dá a profundidade
    // mínima relativa de um nó (lembrando que delta_min_depth = ref_depth -
    // min_ref_depth).
    int depth = 0;
    node* succ = NULL;

    while (
        current
        && (depth + current->delta_ref_depth) - current->delta_min_depth < 0
    ) {
        depth += current->delta_ref_depth;

        if (depth < 0) { // Nó está acima de y
            succ = current;
        }

        // Se não é possível avançar para a direita (porque não existe nó
        // direito, o nó direito não faz parte da árvore splay ou o valor de
        // min_depth é maior ou igual à profundidade de y), e se já encontramos
        // algum nó que estivesse acima de y, aquele nó deve ser o pai esquerdo
        // de y (pois deve ser o maior nó acima de y na árvore de referência).
        // Caso não tenhamos encontrados algum nó acima de y, continuamos a
        // busca no nó esquerdo, sem nos preocupar em diminuir o valor do
        // sucessor encontrado.
        if (
            current->right == NULL
            || current->right->is_splay_root 
            || (depth + current->right->delta_ref_depth) - current->right->delta_min_depth >= 0
        ) {
            if (succ == NULL) {
                current = current->left;
            } else {
                break;
            }
        } else {
            current = current->right;
        }
    }

    return succ;
}

/**
 * @brief Retorna o nó do sucessor (z) de um nó dado (y) seguindo o critério
 *        descrito na seção 3 de [WDS06].
 * 
 * Simétrico a ref_left_parent.
 * 
 * @param y o nó dado.
 * 
 * @return o nó do sucessor.
 */
node* ref_right_parent(node* y) {
    assert(y != NULL);

    node* current = y->right;

    if (
        current == NULL
        || current->is_splay_root
        || current->delta_ref_depth - current->delta_min_depth >= 0
    ) {
        return NULL;
    }

    int depth = 0;
    node* succ = NULL;

    while (
        current
        && (depth + current->delta_ref_depth) - current->delta_min_depth < 0
    ) {
        depth += current->delta_ref_depth;

        if (depth < 0) {
            succ = current;
        }

        if (
            current->left == NULL
            || current->left->is_splay_root 
            || (depth + current->left->delta_ref_depth) - current->left->delta_min_depth >= 0
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
 * Após a execução da função, é garantido que y esteja na raíz de sua árvore
 * splay, o nó à esquerda de y seja seu pai esquerdo (x) na árvore de
 * referência (caso exista) e o nó à direita de y seja seu pai direito (z) na
 * árvore de referência (caso exista).
 * 
 * Além disso, segue que a subárvore direita de x é a subárvore esquerda de y
 * na árvore de referência e a subárvore esquerda de z é a subárvore direita de
 * y na árvore de referência.
 * 
 * Se x ou z não existirem, então o a subárvore esquerda de y será a subárvore
 * esquerda de y na árvore de referência e a subárvore direita de y será a
 * subárvore direita de y na árvore de referência, respectivamente.
 * 
 * @param y nó para o qual trocar o filho preferido (!= NULL).
 */
void switch_preferred(node* y) {
    assert(y != NULL);

    // Fazemos splay de y até a raíz, para garantir que x e z são descendentes
    // de y.
    splay(y, NULL);

    // Predecessor (x) da subárvore esquerda de y (L) na árvore de referência
    node* x = ref_left_parent(y);

    // Sucessor (z) da subárvore direita de y (R) na árvore de referência
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
    // preferido e o outro não.
    assert(!l || !r || l->is_splay_root != r->is_splay_root);

    // Se algum dos nós não existe, não faz sentido trocar
    if (l != NULL) {
        l->is_splay_root ^= true;

        if (z != NULL) {
            maintain_min_depth(z);
        }
    }

    if (r != NULL) {
        r->is_splay_root ^= true;

        if (x != NULL) {
            maintain_min_depth(x);
        }
    }
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

    // Se delta_min_depth = 0, então ref_depth - min_ref_depth = 0, logo
    // ref_depth = min_ref_depth, então a raíz deve ser o nó de profundidade
    // mínima.
    if (root->delta_min_depth == 0) {
        return root;
    }
    
    int relative_min_depth_left =
        root->left->delta_ref_depth - root->left->delta_min_depth;

    int relative_min_depth_right =
        root->right->delta_ref_depth - root->right->delta_min_depth;
        
    if (
        root->left != NULL &&
        (
            root->right == NULL
            || relative_min_depth_left < relative_min_depth_right 
        )
    ) {
        return ref_topmost(root->left);

    } else {
        assert(root->right != NULL);
        assert(
            root->left == NULL
            || relative_min_depth_right < relative_min_depth_left
        );
        
        return ref_topmost(root->right);
    }
}

/**
 * @brief Encontra a raíz da árvore splay de um nó dado.
 * 
 * @param v nó dado (!= NULL).
 * @param depth ponteiro de saída para a profundidade relativa do nó
 *              encontrado na árvore de referência. Deve ser previamente
 *              incializado.
 * 
 * @return o nó raíz da árvore splay correspondente.
 */
node* find_splay_root(node* v, int* depth) {
    assert(v != NULL);

    node* r = v;

    while (!r->is_splay_root) {
        *depth -= r->delta_ref_depth;
        r = r->parent;
    }

    return r;
}

/**
 * @brief Encontra o pai esquerdo de um nó na árvore (ignorando os caminhos
 *        preferidos).
 * 
 * @param root nó raíz de uma árvore splay.
 * @param depth ponteiro de saída para a profundidade relativa do nó
 *              encontrado na árvore de referência. Deve ser previamente
 *              incializado.
 * 
 * @return o nó do pai esquerdo do nó dado.
 */
node* left_parent(node* root, int* depth) {
    assert(root != NULL);

    node* current = root;

    while (current->parent != NULL) {
        *depth -= current->delta_ref_depth;

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
 * @param depth ponteiro de saída para a profundidade relativa do nó
 *              encontrado na árvore de referência. Deve ser previamente
 *              incializado.
 * 
 * @return o nó do pai direito do nó dado.
 */
node* right_parent(node* root, int* depth) {
    node* current = root;

    while (current->parent != NULL) {
        *depth -= current->delta_ref_depth;

        if (current->parent->left == current) {
            return current->parent;
        }
    
        current = current->parent;
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
 * @brief Encontra o nó do filho esquerdo de um nó na árvore de referência.
 * 
 * Descrito na seção 5.2.2 do paper extendido.
 * 
 * @param multiset o conjunto.
 * @param v nó dado (!= NULL).
 * 
 * @return o nó do filho esquerdo na árvore de referência.
 */
node* ref_left_child(tree_multiset* multiset, node* v) {
    assert(v != NULL);
    switch_and_maintain_root(multiset, v);
    
    node* t_l = ref_left_parent(v) == NULL ? v->left : v->left->right;
    
    node* l = NULL;

    if (t_l != NULL) {
        l = ref_topmost(t_l);
        switch_and_maintain_root(multiset, l);
        switch_and_maintain_root(multiset, l);
    }

    switch_and_maintain_root(multiset, v);

    return l;
}

/**
 * @brief Encontra o nó do filho direito de um nó na árvore de referência.
 * 
 * Descrito na seção 5.2.2 do paper extendido.
 * 
 * @param multiset o conjunto.
 * @param v nó dado (!= NULL).
 * 
 * @return o nó do filho direito na árvore de referência.
 */
node* ref_right_child(tree_multiset* multiset, node* v) {
    assert(v != NULL);
    switch_and_maintain_root(multiset, v);
    
    node* t_r = v->right != NULL && v->right->delta_ref_depth < 0
        ? v->right->left
        : v->right;
    
    node* r = NULL;

    if (t_r != NULL) {
        r = ref_topmost(t_r);
        switch_and_maintain_root(multiset, r);
        switch_and_maintain_root(multiset, r);
    }

    switch_and_maintain_root(multiset, v);

    return r;
}

/**
 * @brief Encontra o nó do pai de um nó na árvore de referência.
 * 
 * Descrito na seção 5.2.2 do paper extendido.
 * 
 * @param multiset o conjunto.
 * @param v nó dado (!= NULL).
 * 
 * @return o nó do pai na árvore de referência.
 */
node* ref_parent(tree_multiset* multiset, node* v) {
    switch_and_maintain_root(multiset, v);
    switch_and_maintain_root(multiset, v);

    if (
        v->left != NULL
        && !v->left->is_splay_root
        && v->left->delta_ref_depth == -1
    ) {
        return v->left;
    }
    
    if (
        v->right != NULL
        && !v->right->is_splay_root
        && v->right->delta_ref_depth == -1
    ) {
        return v->right;
    }

    int r_depth = 0;
    node* r = find_splay_root(v, &r_depth);

    int p_depth = r_depth;
    node* l_p = left_parent(r, &p_depth);
    if (l_p != NULL && p_depth == -1) {
        return l_p;
    }

    p_depth = r_depth;
    node* r_p = right_parent(r, &p_depth);
    if (r_p != NULL && p_depth == -1) {
        return r_p;
    }

    return NULL;
}

/**
 * @brief Retorna o nó do predecessor de um valor dado em uma árvore splay
 *        dada.
 * 
 * @param key chave do valor buscado.
 * @param splay_root raíz da árvore splay.
 * @param depth ponteiro de saída para a profundidade relativa do nó
 *              encontrado na árvore de referência. Deve ser previamente
 *              incializado.
 * 
 * @return o nó contendo o predecessor do valor na árvore, ou NULL caso o valor
 *         seja mínimo.
 */
node* predecessor_on_splay(element_t key, node* splay_root, int* depth) {
    node* current = splay_root;

    while (
        current->key >= key
        && current->left
        && !current->left->is_splay_root
    ) {
        current = current->left;
        *depth += current->delta_ref_depth;
    }

    if (current->key >= key) {
        return NULL;
    }

    while (current->right && !current->right->is_splay_root) {
        current = current->right;
        *depth += current->delta_ref_depth;
    }

    return current;
}

/**
 * @brief Retorna o nó do sucessor de um valor dado em uma árvore splay dada.
 * 
 * @param key chave do valor buscado.
 * @param splay_root raíz da árvore splay.
 * @param depth ponteiro de saída para a profundidade relativa do nó
 *              encontrado na árvore de referência. Deve ser previamente
 *              incializado.
 * 
 * @return o nó contendo o sucessor do valor na árvore, ou NULL caso o valor
 *         seja mínimo.
 */
node* successor_on_splay(element_t key, node* splay_root, int* depth) {
    node* current = splay_root;

    while (
        current->key <= key
        && current->right
        && !current->right->is_splay_root
    ) {
        current = current->right;
        *depth += current->delta_ref_depth;
    }

    if (current->key <= key) {
        return NULL;
    }

    while (current->left && !current->left->is_splay_root) {
        current = current->left;
        *depth += current->delta_ref_depth;
    }

    return current;
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
        // Queremos ajustar apenas as arestas não-preferidas
        if (!backtrack->is_splay_root) {
            continue;
        }

        int v_depth = 0;
        node* v = predecessor_on_splay(found->key, backtrack->parent, &v_depth);

        int w_depth = 0;
        node* w = successor_on_splay(found->key, backtrack->parent, &w_depth);

        if (v != NULL && (w == NULL || v_depth < w_depth)) {
            switch_and_maintain_root(multiset, v);
        } else {
            assert(w != NULL);

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

void add_ref_depth(node* v, int s) {
    v->delta_ref_depth += s;

    if (v->left != NULL) {
        v->left->delta_ref_depth -= s;
    }
    
    if (v->right != NULL) {
        v->right->delta_ref_depth -= s;
    }

    maintain_min_depth(v);
}

/**
 * @brief Executa uma rotação virtual para a direita.
 * 
 * @param multiset o conjunto.
 * @param v o filho esquerdo de p na árvore de referência.
 * @param p o nó sendo rotacionado na árvore de referência.
 */
void virtual_rotate_right(tree_multiset* multiset, node* v, node* p) {
    assert(v != NULL);
    assert(p != NULL);

    // Garante que o filho direito de v é o preferido e que v é o filho
    // preferido de p.
    if (v->is_splay_root) {
        switch_and_maintain_root(multiset, p);
    }
    assert(!v->is_splay_root);

    switch_and_maintain_root(multiset, v);

    node* r = p->left;
    if (r != NULL && r->is_splay_root) {
        switch_preferred(v);
    }
    assert(r == NULL || !r->is_splay_root);

    switch_preferred(p);
    if (v->is_splay_root) {
        switch_preferred(p);
    }
    assert(!v->is_splay_root);

    // Modifica as profundidades dos nós apropriados de acordo com a rotação    
    add_ref_depth(v, -1);
    add_ref_depth(p, 1);

    // Subárvore esquerda de v
    node* l_v = v->left;

    node* z = p->right;
    
    // Subárvore direita de p
    node* r_p = z != NULL && z->delta_ref_depth < 0 ? z->left : z;

    assert(
        (r_p->parent == p && r_p->delta_ref_depth == 0)
        || (r_p->parent == z && r_p->delta_ref_depth == 2)
    );

    if (l_v != NULL) {
        l_v->delta_ref_depth--;
        l_v->is_splay_root = true;
        maintain_min_depth(v);
    }

    if (r_p != NULL) {
        r_p->delta_ref_depth++;

        if (z != NULL) {
            maintain_min_depth(z);
        }

        maintain_min_depth(p);
    }

    // Recupera os caminhos preferidos originais
    switch_and_maintain_root(multiset, p);
    switch_and_maintain_root(multiset, v);
}

void virtual_rotate_left(tree_multiset* multiset, node* v, node* p) {
    if (v->is_splay_root) {
        switch_and_maintain_root(multiset, p);
    }
    assert(!v->is_splay_root);

    switch_and_maintain_root(multiset, v);

    node* l = p->right;
    if (l != NULL && l->is_splay_root) {
        switch_preferred(v);
    }
    assert(l == NULL || !l->is_splay_root);

    switch_preferred(p);
    if (v->is_splay_root) {
        switch_preferred(p);
    }
    assert(!v->is_splay_root);
    
    add_ref_depth(v, -1);
    add_ref_depth(p, 1);

    node* r_v = v->right;
    node* x = p->left;
    node* l_p = x != NULL && x->delta_ref_depth < 0 ? x->right : x;

    assert(
        (l_p->parent == p && l_p->delta_ref_depth == 0)
        || (l_p->parent == x && l_p->delta_ref_depth == 2)
    );

    if (r_v != NULL) {
        r_v->delta_ref_depth--;
        r_v->is_splay_root = true;
        maintain_min_depth(v);
    }

    if (l_p != NULL) {
        l_p->delta_ref_depth++;

        if (x != NULL) {
            maintain_min_depth(x);
        }

        maintain_min_depth(p);
    }

    switch_and_maintain_root(multiset, p);
    switch_and_maintain_root(multiset, v);
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
    a.delta_min_depth = 0;
    a.delta_ref_depth = 1;

    b.parent = &e;
    b.left = &a;
    b.right = &c;
    b.is_splay_root = false;
    b.delta_min_depth = 0;
    b.delta_ref_depth = 1;
    
    c.parent = &b;
    c.right = &d;
    c.is_splay_root = true;
    c.delta_min_depth = 0;
    c.delta_ref_depth = 1;

    d.parent = &c;
    d.is_splay_root = false;
    d.delta_min_depth = 0;
    d.delta_ref_depth = 1;

    e.parent = NULL;
    e.left = &b;
    e.right = &h;
    e.is_splay_root = true;
    e.delta_min_depth = 0;
    e.delta_ref_depth = 1;
    
    f.parent = &h;
    f.right = &g;
    f.is_splay_root = false;
    f.delta_min_depth = 0;
    f.delta_ref_depth = 1;

    g.parent = &f;
    g.is_splay_root = false;
    g.delta_min_depth = 0;
    g.delta_ref_depth = 1;
    
    h.parent = &e;
    h.left = &f;
    h.right = &i;
    h.is_splay_root = true;
    h.delta_min_depth = 0;
    h.delta_ref_depth = 1;
    
    i.parent = &h;
    i.right = &j;
    i.is_splay_root = true;
    i.delta_min_depth = 0;
    i.delta_ref_depth = 1;
    
    j.parent = &i;
    j.is_splay_root = false;
    j.delta_min_depth = 0;
    j.delta_ref_depth = 1;

    tree_multiset s = { &e };

    printf("%p\n", ref_left_child(&s, &c));
    printf("%p\n", ref_right_child(&s, &c));
    printf("%"PRIu64"\n", ref_parent(&s, &c)->key);

    printf("%"PRIu64"\n", ref_left_child(&s, &b)->key);
    printf("%"PRIu64"\n", ref_right_child(&s, &b)->key);
    printf("%"PRIu64"\n", ref_parent(&s, &b)->key);

    virtual_rotate_left(&s, &c, &b);
    
    printf("%"PRIu64"\n", ref_left_child(&s, &c)->key);
    printf("%p\n", ref_right_child(&s, &c));
    printf("%"PRIu64"\n", ref_parent(&s, &c)->key);

    printf("%"PRIu64"\n", ref_left_child(&s, &b)->key);
    printf("%p\n", ref_right_child(&s, &b));
    printf("%"PRIu64"\n", ref_parent(&s, &b)->key);

    printf("%"PRIu64"\n", ref_left_child(&s, &e)->key);
    printf("%"PRIu64"\n", ref_right_child(&s, &e)->key);
    printf("%p\n", ref_parent(&s, &e));

    virtual_rotate_right(&s, &c, &e);
    
    printf("%"PRIu64"\n", ref_left_child(&s, &c)->key);
    printf("%"PRIu64"\n", ref_right_child(&s, &c)->key);
    printf("%p\n", ref_parent(&s, &c));

    printf("%"PRIu64"\n", ref_left_child(&s, &e)->key);
    printf("%"PRIu64"\n", ref_right_child(&s, &e)->key);
    printf("%"PRIu64"\n", ref_parent(&s, &e)->key);

    return 0;
}
