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
#include <limits.h>

#include <time.h>
#include <stdlib.h>

/**
 * @brief Enumeração de cores de nós da árvore de referência. 
 */
typedef enum color {
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

    // Número de remoções necessários na subárvore com raíz no nó para tornar a
    // árvore legal
    size_t diff_cool;

    // Filho esquerdo
    struct tree_multiset_node* left;

    // Filho direito
    struct tree_multiset_node* right;

    // Nó pai (NULL para a raíz)
    struct tree_multiset_node* parent;

    // Diferença de profundidade do nó na árvore de referência em relação ao
    // pai na árvore multi-splay.
    int_fast16_t delta_ref_depth;

    // Diferença de valor de ref_depth mínimo na subárvore-splay do nó em
    // relação ao pai.
    int_fast16_t delta_min_depth;

    // Determina se a conexão com o nó pai é tracejada (true) ou sólida
    // (false). Uma conexão tracejada significa que este nó é raíz de uma
    // árvore splay, ou ainda o nó filho não-preferido.
    bool is_splay_root : 1;

    // Cor do nó na árvore de referência
    color color : 1;
} node;

struct tree_multiset {
    node* root;
};

/**
 * @brief Corrige o valor auxiliar delta_min_depth para um nó após alguma
 *        rotação ou alteração no filho preferido.
 * 
 * Descrito em [ST85], seção 6.
 * 
 * @param root nó envolvido na rotação.
 */
void maintain_min_depth(node* root) {
    assert(root != NULL);

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
 * @brief Corrige os valores auxiliares dos nós envolvidos em uma rotação.
 * 
 * @param p nó rotacionado.
 * @param q filho rotacionado.
 * @param y filho trocado (na rotação direita, o filho direito de q, na
 *          esquerda o filho esquerdo de q).
 */
void maintain_auxiliary_values(node* p, node* q, node* y) {
    assert(p != NULL);
    assert(q != NULL);

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
 * @brief 
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

    assert(y == NULL || !q->is_splay_root || y->is_splay_root);
    
    q->left = p;
    p->right = y;

    if (y != NULL) {
        q->diff_cool -= y->diff_cool;
    }
    p->diff_cool -= q->diff_cool;
    q->diff_cool += p->diff_cool;

    maintain_parents(o, p, q, y);
    maintain_auxiliary_values(p, q, y);
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

    assert(y == NULL || !q->is_splay_root || y->is_splay_root);

    q->right = p;
    p->left = y;

    if (y != NULL) {
        q->diff_cool -= y->diff_cool;
    }
    p->diff_cool -= q->diff_cool;
    q->diff_cool += p->diff_cool;

    maintain_parents(o, p, q, y);
    maintain_auxiliary_values(p, q, y);
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
void splay(node* subject, node* root)
    __attribute__((hot));

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
 * @brief Determina se a subárvore de um nó (p) pode conter um antecessor de
 *        outro nó (y) na árvore de referência.
 * 
 * @param p nó sendo testado.
 * @param depth profundidade relativa de p em relação a y.
 * 
 * @return verdadeiro se é possível que p seja um antecessor de y ou contenha
 *         um antecessor de y na árvore de referência em sua subárvore.
 */
bool is_ref_parent_candidate(node* p, int depth) {
    // delta_ref_depth - delta_min_depth = min_depth relativo a y, se for maior
    // ou igual a 0, não existem nós com profundidade menor que y na subárvore,
    // i.e. não existe pai esquerdo.
    return p != NULL
            && !p->is_splay_root
            && depth + p->delta_ref_depth - p->delta_min_depth < 0;
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
    
    // Acumulador de profundidade relativa à subárvore esquerda de y.
    int depth = 0;

    current = (node*) ((size_t) current * is_ref_parent_candidate(current, depth));

    node* pred = NULL;
    while (is_ref_parent_candidate(current, depth)) {
        depth += current->delta_ref_depth;

        // Queremos o maior nó com profundidade menor que y
        if (depth < 0 && (pred == NULL || current->key > pred->key)) {
            pred = current;
        }

        // Idealmente, queremos sempre aumentar o valor do nó. Caso não seja
        // possível, aceitamos diminuir o valor para possivelmente encontrar um
        // antecessor maior que o atual que esteja acima de y na árvore de
        // referência.
        if (!is_ref_parent_candidate(current->right, depth)) {
            if (pred != NULL && pred->key > current->key) {
                return pred;
            }

            current = current->left;
        } else {
            current = current->right;
        }
    }

    return pred;
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
    int depth = 0;

    current = (node*) ((size_t) current * is_ref_parent_candidate(current, depth));

    node* succ = NULL;
    while (is_ref_parent_candidate(current, depth)) {
        depth += current->delta_ref_depth;

        if (depth < 0 && (succ == NULL || current->key < succ->key)) {
            succ = current;
        }

        if (!is_ref_parent_candidate(current->left, depth)) {
            if (succ != NULL && succ->key < current->key) {
                return succ;
            }

            current = current->right;
        } else {
            current = current->left;
        }
    }

    return succ;
}

/**
 * @brief Arranja as condições necessárias para realizar uma troca de filho
 *        preferido em tempo constante.
 * 
 * @param y nó cujo filho preferido será trocado. 
 * @param x ponteiro de saída para o pai esquerdo de y na árvore de referência
 *          caso esteja na árvore splay de y. Recebe NULL caso não exista ou
 *          não esteja na árvore splay de y.
 * @param z idem x, mas recebe o pai direito de y na árvore de referência.
 * @param l ponteiro de saída para o nó raíz da árvore splay correspondente à
 *          subárvore esquerda de y na árvore de referência.
 * @param r idem l, mas para a subárvore direita de y na árvore de referência.
 */
void prepare_switch(
    node* y,
    node** x,
    node** z,
    node** l,
    node** r
) {
    assert(y != NULL);

    // Fazemos splay de y até a raíz, para garantir que x e z são descendentes
    // de y.
    splay(y, NULL);

    // Predecessor (x) da subárvore esquerda de y (L) na árvore de referência
    *x = ref_left_parent(y);

    // Sucessor (z) da subárvore direita de y (R) na árvore de referência
    *z = ref_right_parent(y);

    // Subimos x e z até que sejam filhos de y, de forma que garantimos que a
    // direita de a é L e a subárvore esquerda de z é R.
    // Se x e/ou z não existem, então a subárvore esquerda de y deve ser L e
    // a subárvore direita de y deve ser R (seção 3.3.3 do paper extendido).
    if (*x != NULL) {
        splay(*x, y);
        *l = (*x)->right;
    } else {
        *l = y->left;
    }

    if (*z != NULL) {
        splay(*z, y);
        *r = (*z)->left;
    } else {
        *r = y->right;
    }
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
 * Caso o nó dado tenha apenas um filho, esta função sempre marca ele como
 * filho preferido.
 * 
 * @param y nó para o qual trocar o filho preferido (!= NULL).
 */
void switch_preferred(node* y) __attribute__((hot));

void switch_preferred(node* y) {
    node *x, *z, *l, *r;
    prepare_switch(y, &x, &z, &l, &r);

    // Invariante: durante uma troca, um dos nós trocados deve ser um caminho
    // preferido e o outro não.
    assert(!l || !r || l->is_splay_root != r->is_splay_root);

    if (l != NULL && r != NULL) {
        l->is_splay_root ^= true;
        r->is_splay_root ^= true;
    } else if (l != NULL) {
        l->is_splay_root = false;
    } else if (r != NULL) {
        r->is_splay_root = false;
    }

    if (z != NULL) {
        maintain_min_depth(z);
    }

    if (x != NULL) {
        maintain_min_depth(x);
    }

    maintain_min_depth(y);
}

/**
 * @brief Troca o filho preferido de um nó duas vezes (i.e. não troca). Isso é
 *        útil apenas para reestruturação da árvore.
 * 
 * @param y o nó.
 */
void switch_preferred_twice(node* y) {
    node *x, *z, *l, *r;
    prepare_switch(y, &x, &z, &l, &r);
}

/**
 * @brief Enumeração de direções de troca de filho preferido. 
 */
typedef enum direction {
    LTR,    // Da esquerda para a direita
    RTL     // Da direita para a esquerda
} direction;

/**
 * @brief Troca o filho preferido de um nó em uma direção dada.
 * 
 * Diferente da função switch_preferred, esta função pode fazer com que o único
 * filho do nó dado seja marcado como não-preferido. Isso é importante durante
 * as rotações virtuais.
 * 
 * @param y nó cujo filho deve ser trocado.
 * @param dir direção da troca.
 */
void switch_with_direction(node* y, direction dir) {
    node *x, *z, *l, *r;
    prepare_switch(y, &x, &z, &l, &r);

    assert(
        l == NULL
        || r == NULL
        || (l->is_splay_root == (dir != LTR)
            && r->is_splay_root == (dir != RTL))
    );

    if (l != NULL) {
        l->is_splay_root = (dir == LTR);
    }
    
    if (r != NULL) {
        r->is_splay_root = (dir == RTL);
    }

    if (z != NULL) {
        maintain_min_depth(z);
    }

    if (x != NULL) {
        maintain_min_depth(x);
    }

    maintain_min_depth(y);
}

/**
 * @brief Corrige o ponteiro da raíz de uma árvore.
 * 
 * @param multiset ponteiro para a árvore.
 * @param candidate nó que possivelmente se tornou a raíz da árvore.
 */
void maintain_root(tree_multiset* multiset, node* candidate) {
    if (candidate->parent == NULL) {
        multiset->root = candidate;
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

    node* current = root;

    // Se delta_min_depth = 0, então ref_depth - min_ref_depth = 0, logo
    // ref_depth = min_ref_depth, então o atual deve ser o nó de profundidade
    // mínima.
    while (current->delta_min_depth != 0) {
        int relative_min_depth_left = root->left != NULL
            ? root->left->delta_ref_depth - root->left->delta_min_depth
            : INT_MAX;

        int relative_min_depth_right = root->right != NULL
            ? root->right->delta_ref_depth - root->right->delta_min_depth
            : INT_MAX;
            
        if (relative_min_depth_left < relative_min_depth_right) {
            current = current->left;
        } else {
            assert(root->right != NULL);
            assert(
                root->left == NULL
                || relative_min_depth_right < relative_min_depth_left
            );
            
            current = current->right;
        }
    }

    return current;
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
    switch_preferred(v);
    
    node* t_l = v->left != NULL && v->left->delta_ref_depth < 0
        ? v->left->right
        : v->left;
    
    node* l = NULL;

    if (t_l != NULL) {
        l = ref_topmost(t_l);
    }

    switch_preferred(v);

    maintain_root(multiset, v);

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
    switch_preferred(v);

    node* t_r = v->right != NULL && v->right->delta_ref_depth < 0
        ? v->right->left
        : v->right;
    
    node* r = NULL;

    if (t_r != NULL) {
        r = ref_topmost(t_r);
    }

    switch_preferred(v);
    maintain_root(multiset, v);

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
    switch_preferred_twice(v);
    maintain_root(multiset, v);

    if (v->left != NULL && v->left->delta_ref_depth == -1) {
        return v->left;
    }
    
    if (v->right != NULL && v->right->delta_ref_depth == -1) {
        return v->right;
    }

    int depth = 0;
    node* current = v->parent;
    while (current != NULL) {
        depth -= current->delta_ref_depth;

        if (depth == -1) {
            return current;
        }

        current = current->parent;
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
node* predecessor_on_splay(node* root, element_t key, int* depth) {
    if (root == NULL) {
        return NULL;
    }

    *depth += root->delta_ref_depth;

    if (root->key == key) {
        node* current = root->left;

        if (current == NULL || current->is_splay_root) {
            *depth -= root->delta_ref_depth;
            return NULL;
        }

        *depth += current->delta_ref_depth;

        while (current->right != NULL && !current->right->is_splay_root) {
            current = current->right;
            *depth += current->delta_ref_depth;
        }

        return current;

    } else if (root->key < key) {
        node* rec = NULL;
        
        if (root->right != NULL && !root->right->is_splay_root) {
            rec = predecessor_on_splay(root->right, key, depth);
        }
        
        return rec == NULL ? root : rec;

    } else if (root->left != NULL && !root->left->is_splay_root) {
        node* rec = predecessor_on_splay(root->left, key, depth);

        if (rec == NULL) {
            *depth -= root->delta_ref_depth;
        }

        return rec;
    }

    *depth -= root->delta_ref_depth;
    return NULL;
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
node* successor_on_splay(node* root, element_t key, int* depth) {
    if (root == NULL) {
        return NULL;
    }

    *depth += root->delta_ref_depth;

    if (root->key == key) {
        node* current = root->right;

        if (current == NULL || current->is_splay_root) {
            *depth -= root->delta_ref_depth;
            return NULL;
        }
        
        *depth += current->delta_ref_depth;

        while (current->left != NULL && !current->left->is_splay_root) {
            current = current->left;
            *depth += current->delta_ref_depth;
        }

        return current;

    } else if (root->key > key) {
        node* rec = NULL;
        
        if (root->left != NULL && !root->left->is_splay_root) {
            rec = successor_on_splay(root->left, key, depth);
        }
        
        return rec == NULL ? root : rec;

    } else if (root->right && !root->right->is_splay_root) {
        node* rec = successor_on_splay(root->right, key, depth);
        
        if (rec == NULL) {
            *depth -= root->delta_ref_depth;
        }

        return rec;
    }

    *depth -= root->delta_ref_depth;
    return NULL;
}

// Tamanho máximo da stack de trocas de filho preferido para um percurso.
#define SWITCH_STACK_MAX 256

/**
 * @brief Percorre a árvore buscando um valor dado constrói uma pilha de nós
 *        cujo filho preferido deve ser trocado durante a operação de
 *        multi-splay para o valor.
 * 
 * Descrito em [WDS06], seção 3.
 * 
 * @param root a raíz do conjunto.
 * @param key o valor do elemento para o que o multi-splay será feito.
 * @param stack ponteiro para um vetor que será usado como pilha.
 * @param stack_p ponteiro para um valor que contém o índice do último elemento
 *                na pilha.
 * 
 * Caso a pilha esteja vazia, stack_p contém -1 ao final da execução da função.
 * 
 * @return o nó encontrado para o valor dado na árvore, ou seu pai caso o valor
 *         não exista no conjunto.
 */
node* find_and_record_switches(
    node* root,
    element_t key,
    node* stack[],
    int* stack_p
) {
    *stack_p = 0;
    
    if (root == NULL || root->key == key) {
        *stack_p = -1;
        return root;
    }

    node* current = root;
    while (current != NULL && current->key != key) {
        // Verifica-se o limite da stack por educação, mas não deve acontecer:
        // a árvore tem no máximo 2 lg n árvores splay (pois a árvore de
        // referência é uma rubro-negra e cada nó tem exatamente um filho
        // preferido), das quais cada uma gera no máximo uma entrada na pilha.
        // Para estourar esse valor, teríamos que ter mais que 2^128 elementos
        // distintos na árvore, o que certamente não é possível usando valores
        // de 64 bits.
        assert(*stack_p >= SWITCH_STACK_MAX);

        // A troca acontece no predecessor ou no sucessor do valor buscado na
        // árvore splay atual, dependendo de qual for mais profundo na árvore
        // de referência.
        int v_depth = 0;
        node* v = predecessor_on_splay(current, key, &v_depth);

        int w_depth = 0;
        node* w = successor_on_splay(current, key, &w_depth);

        if (v != NULL && (w == NULL || v_depth > w_depth)) {
            stack[*stack_p] = v;
        } else {
            assert(w != NULL);
            stack[*stack_p] = w;
        }

        // Avançamos até a raíz da próxima árvore splay, caso exista
        do {
            if (key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        } while (
            current != NULL
            && current->key != key
            && !current->is_splay_root
        );

        if (
            current != NULL
            && (current->key != key || current->is_splay_root)
        ) {
            (*stack_p)++;
        }
    }

    (*stack_p)--;

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
node* multi_splay(tree_multiset* multiset, element_t key) {
    node* stack[SWITCH_STACK_MAX];
    int stack_p;

    node* found =
        find_and_record_switches(multiset->root, key, stack, &stack_p);

    if (found == NULL) {
        return NULL;
    }

    while (stack_p >= 0) {
        switch_preferred(stack[stack_p--]);
    }

    for (
        node* backtrack = found;
        backtrack != NULL && backtrack->parent != NULL;
        backtrack = backtrack->parent
    ) {
        assert(!backtrack->is_splay_root);
    }

    switch_preferred(found);
    maintain_root(multiset, found);

    return found;
}

/**
 * @brief Altera a profundidade de um nó na árvore de referência sem afetar
 *        sua subárvore na MST.
 * 
 * @param v nó cuja profundidade deve ser alterada.
 * @param s valor a ser adicionado à profundidade (pode ser negativo).
 */
void add_ref_depth(node* v, int s) {
    assert(v != NULL);

    v->delta_ref_depth += s;

    if (v->left != NULL) {
        v->left->delta_ref_depth -= s;
    }
    
    if (v->right != NULL) {
        v->right->delta_ref_depth -= s;
    }
}

/**
 * @brief Prepara a MST para uma rotação virtual à direita.
 * 
 * Descrito em [WDS06], seção 6.5.
 * 
 * @param multiset a MST.
 * @param v o filho esquerdo de p na árvore de referência.
 * @param p o nó sendo rotacionado na árvore de referência.
 */
void prepare_virtual_right_rotation(
    tree_multiset* multiset,
    node* v,
    node* p
) {
    node* r = ref_right_child(multiset, v);
    if (r == NULL || !r->is_splay_root) {
        switch_with_direction(v, RTL);
    }
    switch_with_direction(v, LTR);
    assert(r == NULL || !r->is_splay_root);

    splay(p, NULL);
    if (!v->is_splay_root) {
        switch_with_direction(p, LTR);
    }
    switch_with_direction(p, RTL);

    assert(!v->is_splay_root);

    maintain_root(multiset, p);
}

/**
 * @brief Altera as profundidades dos nós na árvore de referência de forma a
 *        simular uma rotação à direita.
 * 
 * @param v filho esquerdo de p na árvore de referência.
 * @param p nó sendo rotacionado na árvore de referência.
 */
void adjust_ref_depths_right(node* v, node* p) {
    add_ref_depth(v, -1);
    add_ref_depth(p, 1);

    // Subárvore esquerda de v
    node* l_v = v->left;

    if (l_v != NULL) {
        l_v->delta_ref_depth--;
        maintain_min_depth(v);
    }

    node* z = p->right;
    
    // Subárvore direita de p
    node* r_p = z != NULL && z->delta_ref_depth < 0 ? z->left : z;

    if (r_p != NULL) {
        r_p->delta_ref_depth++;

        if (z != NULL) {
            maintain_min_depth(z);
        }

        maintain_min_depth(p);
    }
}

/**
 * @brief Executa uma rotação virtual para a direita.
 * 
 * Descrito em [WDS06], seção 6.5.
 * 
 * @param multiset o conjunto.
 * @param v o filho esquerdo de p na árvore de referência.
 * @param p o nó sendo rotacionado na árvore de referência.
 */
void virtual_rotate_right(tree_multiset* multiset, node* v, node* p) {
    assert(v != NULL);
    assert(p != NULL);

    assert(ref_parent(multiset, v) == p);
    assert(v == ref_left_child(multiset, p));

    // Garante que o filho direito de v é o preferido e que v é o filho
    // preferido de p.
    prepare_virtual_right_rotation(multiset, v, p);

    // Modifica as profundidades dos nós apropriados de acordo com a rotação
    adjust_ref_depths_right(v, p);
    
    // Corrige as cores
    color color_p = p->color;
    v->color = color_p;
    p->color = RED;

    maintain_root(multiset, v);

    assert(ref_right_child(multiset, v) == p);
    assert(ref_parent(multiset, p) == v);
}

/**
 * @brief Prepara a MST para uma rotação virtual à esquerda.
 * 
 * Descrito em [WDS06], seção 6.5.
 * 
 * @param multiset a MST.
 * @param v o filho direito de p na árvore de referência.
 * @param p o nó sendo rotacionado na árvore de referência.
 */
void prepare_virtual_left_rotation(
    tree_multiset* multiset,
    node* v,
    node* p
) {
    node* l = ref_left_child(multiset, v);
    if (l == NULL || !l->is_splay_root) {
        switch_with_direction(v, LTR);
    }
    switch_with_direction(v, RTL);
    assert(l == NULL || !l->is_splay_root);

    splay(p, NULL);
    if (!v->is_splay_root) {
        switch_with_direction(p, RTL);
    }
    switch_with_direction(p, LTR);

    assert(!v->is_splay_root);

    maintain_root(multiset, p);
}

/**
 * @brief Altera as profundidades dos nós na árvore de referência de forma a
 *        simular uma rotação à esquerda.
 * 
 * @param v filho direito de p na árvore de referência.
 * @param p nó sendo rotacionado na árvore de referência.
 */
void adjust_ref_depths_left(node* v, node* p) {
    add_ref_depth(v, -1);
    add_ref_depth(p, 1);

    // Subárvore direita de v
    node* r_v = v->right;

    if (r_v != NULL) {
        r_v->delta_ref_depth--;
        maintain_min_depth(v);
    }

    node* x = p->left;
    
    // Subárvore esquerda de p
    node* l_p = x != NULL && x->delta_ref_depth < 0 ? x->right : x;

    if (l_p != NULL) {
        l_p->delta_ref_depth++;

        if (x != NULL) {
            maintain_min_depth(x);
        }

        maintain_min_depth(p);
    }
}

/**
 * @brief Executa uma rotação virtual para a esquerda.
 * 
 * Descrito em [WDS06], seção 6.5.
 * 
 * @param multiset o conjunto.
 * @param v o filho direito de p na árvore de referência.
 * @param p o nó sendo rotacionado na árvore de referência.
 */
void virtual_rotate_left(tree_multiset* multiset, node* v, node* p) {
    assert(v != NULL);
    assert(p != NULL);
    
    assert(ref_parent(multiset, v) == p);
    assert(v == ref_right_child(multiset, p));

    // Garante que o filho esquerdo de v é o preferido e que v é o filho
    // preferido de p.
    prepare_virtual_left_rotation(multiset, v, p);

    // Modifica as profundidades dos nós apropriados de acordo com a rotação
    adjust_ref_depths_left(v, p);

    // Corrige as cores
    color color_p = p->color;
    v->color = color_p;
    p->color = RED;

    maintain_root(multiset, v);

    assert(ref_left_child(multiset, v) == p);
    assert(ref_parent(multiset, p) == v);
}

/**
 * @return verdadeiro se o nó dado é vermelho na árvore de referência.
 */
bool is_red(node* v) {
    return v != NULL && v->color == RED;
}

/**
 * @return verdadeiro se o nó dado é preto na árvore de referência.
 */
bool is_black(node* v) {
    return v == NULL || v->color == BLACK;
}

/**
 * @brief Rebalanceia a árvore de referência após uma inserção.
 * 
 * Implementa as regras de uma árvore rubro-negra esquerdista.
 * 
 * @param multiset a MST.
 * @param start o nó a partir do qual começar a rebalancear (de cima para
 *              baixo).
 */
void virtual_rebalance(tree_multiset* multiset, node* start) {
    node* current = start;

    while (current != NULL) {
        node* x = ref_left_child(multiset, current);
        node* y = ref_right_child(multiset, current);

        if (is_black(x) && is_red(y)) {
            virtual_rotate_left(multiset, y, current);

            x = current;
            current = y;
            y = ref_right_child(multiset, current);
        }

        node* grandchild = ref_left_child(multiset, x);
        if (is_red(x) && is_red(grandchild)) {
            virtual_rotate_right(multiset, x, current);

            y = current;
            current = x;
            x = grandchild;
        }

        if (is_red(x) && is_red(y)) {
            x->color = BLACK;
            y->color = BLACK;
            current->color = RED;
        }

        node* parent = ref_parent(multiset, current);

        // Mantém a raíz preta
        if (parent == NULL) {
            current->color = BLACK;
            break;
        }

        current = parent;
    }
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
    v->is_splay_root = true;
    v->delta_ref_depth = 1;
    v->delta_min_depth = 0;
    v->color = RED;
    return v;
}

/**
 * @brief Busca por um elemento mantendo o pai esquerdo e direito, bem como
 *        suas profundidades na árvore de referência.
 * 
 * @param root raíz a partir da qual buscar.
 * @param key elemento sendo procurado.
 * @param x ponteiro de saída para o nó do pai esquerdo.
 * @param x_depth ponteiro de saída para a profundidade do pai esquerdo.
 * @param z idem pred para o pai direito.
 * @param z_depth idem x_depth para o pai direito.
 * 
 * @return um ponteiro para o nó encontrado com a chave buscada, ou NULL caso
 *         ele não exista na árvore.
 */
node* find_with_parents(
    node* root,
    element_t key,
    node** x,
    int* x_depth,
    node** z,
    int* z_depth
) {
    node* current = root;
    
    *x = NULL;
    *x_depth = 0;

    *z = NULL;
    *z_depth = 0;

    int depth = 0;
    while (current && current->key != key) {
        depth += current->delta_ref_depth;

        if (key < current->key) {
            *z = current;
            *z_depth = depth;
            current = current->left;
        } else {
            *x = current;
            *x_depth = depth;
            current = current->right;
        }
    }

    return current;
}

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

void propagate_diff_cool(node* found) {
    node* current = found->parent;

    while (current != NULL) {
        maintain_diff_cool(current);
        current = current->parent;
    }
}

/**
 * @brief Insere um nó na MST dados seus pais direito e equerdo.
 * 
 * Descrito em [WDS06], seção 6.4.
 * 
 * @param multiset a MST.
 * @param x o pai esquerdo do nó.
 * @param x_depth a profundidade de x na árvore de referência.
 * @param z o pai direito do nó.
 * @param z_depth a profundidade de z na árvore de referência.
 * @param created nó a ser inserido.
 */
void insert_with_parents(
    tree_multiset* multiset,
    node* x,
    int x_depth,
    node* z,
    int z_depth,
    node* created
) __attribute__((hot));

void insert_with_parents(
    tree_multiset* multiset,
    node* x,
    int x_depth,
    node* z,
    int z_depth,
    node* created
) {
    node* parent;
    
    if (x != NULL && x_depth > z_depth) {
        parent = x;
        multi_splay(multiset, x->key);

        assert(x->right == z);

        if (z != NULL) {
            assert(z->left == NULL);

            z->left = created;
            created->parent = z;
            created->delta_ref_depth = x_depth + 1 - z_depth;
        } else {
            assert(x->right == NULL);

            x->right = created;
            created->parent = x;
        }
    } else {
        assert(z != NULL);
        assert(z_depth > x_depth);

        parent = z;
        multi_splay(multiset, z->key);

        assert(z->left == x);

        if (x != NULL) {
            assert(x->right == NULL);

            x->right = created;
            created->parent = x;
            created->delta_ref_depth = z_depth + 1 - x_depth;
        } else {
            assert(z->left == NULL);

            z->left = created;
            created->parent = z;
        }
    }

    propagate_diff_cool(created);
    virtual_rebalance(multiset, parent);
}

tree_multiset* multiset_init() {
    tree_multiset* multiset = (tree_multiset*) xmalloc(sizeof(tree_multiset));   
    multiset->root = NULL;
    return multiset;    
}

// Complexidade: O(log^2 n) pior caso       [WDS06, teorema 4.1]
//               O(log log n)-competitiva   [WDS06, teorema 4.2]
//               O(log n) amortizado        [WDS06, teorema 4.3]
//
// Em particular, se a sequência de acessos for sequencial, o tempo gasto ao
// todo é O(n).
size_t multiset_count(tree_multiset* multiset, element_t elem) {
    node* found = multi_splay(multiset, elem);
    
    if (found == NULL) {
        return 0;
    }

    return found->count;
}

void multiset_insert(tree_multiset* multiset, element_t key) {
    if (multiset->root == NULL) {
        multiset->root = make_node(key);
        multiset->root->color = BLACK;
        return;
    }

    node *x, *z;
    int x_depth, z_depth;

    node* found = find_with_parents(
        multiset->root,
        key,
        &x, &x_depth,
        &z, &z_depth
    );

    if (found != NULL) {
        found->count++;

        maintain_diff_cool(found);
        propagate_diff_cool(found);
        multi_splay(multiset, found->key);
        
    } else {
        node* created = make_node(key);
        
        insert_with_parents(multiset, x, x_depth, z, z_depth, created);
    }
}

void multiset_destroy(tree_multiset* multiset) {
    destroy_node(multiset->root);
}

size_t multiset_diff_cool(tree_multiset* multiset) {
    return multiset->root->diff_cool;
}
