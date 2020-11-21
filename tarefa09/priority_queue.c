/**
 * @file priority_queue.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação de fila de prioridade com remoção com Beap [MS80].
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa09.html
 * 
 * [MS80] MUNRO, J. Ian; SUWANDA, Hendra.
 *        Implicit data structures for fast search and update.
 *        Journal of Computer and System Sciences, v. 21, n. 2, p. 236-250,
 *        1980. https://doi.org/10.1016/0022-0000(80)90037-9
 */

#include "priority_queue.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <assert.h>
#include <math.h>

#include "map.h"
#include "xmalloc.h"

struct priority_queue {
    const customer* customers[249];
    map* ratings;
    size_t size;
    size_t height;
};

/**
 * @brief Calcula o índice no array do primeiro elemento com dada profundidade
 *        no Beap.
 * 
 * @param depth profundidade desejada.
 * 
 * @return índice no array,
 */
int first(int depth) {
    return depth * (depth - 1) / 2;
}

/**
 * @brief Calcula o índice no array do último elemento com dada profundidade no
 *        Beap.
 * 
 * @param depth profundidade desejada.
 * 
 * @return índice no array,
 */
int last(int depth) {
    return first(depth + 1) - 1;
}

/**
 * @brief Calcula o índice do pai esquerdo de um elemento com dada profundidade
 *        no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do pai esquerdo.
 */
int left_parent(int i, int depth) {
    return i - depth;
}

/**
 * @brief Calcula o índice do pai direito de um elemento com dada profundidade
 *        no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do pai direito.
 */
int right_parent(int i, int depth) {
    return i - depth + 1;
}

/**
 * @brief Calcula o índice do filho esquerdo de um elemento com dada
 *        profundidade no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do filho esquerdo.
 */
int left_child(int i, int depth) {
    return i + depth;
}

/**
 * @brief Calcula o índice do filho direito de um elemento com dada
 *        profundidade no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do filho direito.
 */
int right_child(int i, int depth) {
    return i + depth + 1;
}

/**
 * @brief Sobe um elemento no Beap até que esteja na posição apropriada.
 * 
 * @param q o Beap.
 * @param c o cliente sendo movido.
 * @param i o índice do elemento no Beap.
 * @param depth a profundidade do elemento no Beap.
 */
void sift_up(priority_queue* q, const customer* c, int i, int depth) {
    for (; depth > 1; depth--) {

        // Calculamos o índice e prioridade dos pais do elemento, usando
        // INFINITY caso algum dois pais seja inválido. Isso tem o efeito de
        // realmente ignorar aquele pai, porque INFINITY > x para todo x.
        int l_i = -1, r_i = -1;
        double l_rating, r_rating;
        
        if (i != first(depth)) {
            l_i = left_parent(i, depth);
            l_rating = q->customers[l_i]->rating;
        } else {
            l_rating = INFINITY;
        }

        if (i != last(depth)) {
            r_i = right_parent(i, depth);
            r_rating = q->customers[r_i]->rating;
        } else {
            r_rating = INFINITY;
        }

        // Se o elemento é maior qualquer um dos pais, trocamos ele pelo menor
        // dos pais
        if (c->rating > l_rating || c->rating > r_rating) {
            if (l_rating < r_rating) {
                assert(l_i != -1);

                q->customers[i] = q->customers[l_i];
                i = l_i;
            } else {
                assert(r_i != -1);

                q->customers[i] = q->customers[r_i];
                i = r_i;
            }

        // Se não, encontramos o lugar correto para o elemento
        } else {
            q->customers[i] = c;
            return;
        }
    }

    // Caso especial: a função não se comporta muito bem para depth = 1 (em
    // específico por conta da definição de right_parent), então tratamos o
    // caso onde o elemento é o maior separadamente.
    if (c->rating > q->customers[0]->rating) {
        q->customers[i] = q->customers[0];
        q->customers[0] = c;
    } else {
        q->customers[i] = c;
    }
}

/**
 * @brief Desce um elemento no Beap até que esteja na posição apropriada.
 * 
 * @param q o Beap.
 * @param c o cliente sendo movido.
 * @param i o índice do elemento no Beap.
 * @param depth a profundidade do elemento no Beap.
 */
void sift_down(priority_queue* q, const customer* c, int i, int depth) {
    for (; depth <= q->height; depth++) {
        int l_i = left_child(i, depth), r_i;
        double l_rating, r_rating;
        
        if (l_i < q->size) {
            l_rating = q->customers[l_i]->rating;

            r_i = right_child(i, depth);

            if (r_i < q->size) {
                r_rating = q->customers[r_i]->rating;
            } else {
                r_rating = -INFINITY;
            }
        } else {
            l_rating = r_rating = -INFINITY;
        }

        if (c->rating < l_rating || c->rating < r_rating) {
            if (l_rating > r_rating) {
                q->customers[i] = q->customers[l_i];
                i = l_i;
            } else {
                q->customers[i] = q->customers[r_i];
                i = r_i;
            }
        } else {
            q->customers[i] = c;
            return;
        }
    }

    q->customers[i] = c;
}

/**
 * @brief Remove um elemento do Beap dado seu índice e profundidade.
 * 
 * @param q o Beap.
 * @param i o índice do elemento no Beap.
 * @param depth a profundidade do elemento no Beap.
 */
void delete_at(priority_queue* q, int i, int depth) {
    q->size--;

    const customer* c = q->customers[q->size];

    // No máximo um dos dois tem efeito, mas não tem vantagem verificar antes
    // qual deve acontecer (a lógica seria a mesma da função).
    sift_up(q, c, i, depth);
    sift_down(q, c, i, depth);
    
    if (q->size < first(q->height)) {
        q->height--;
    }
}

/**
 * @brief Encontra o índice e profundidade
 * 
 * @param q 
 * @param rating 
 * @param depth 
 * @return int 
 */
int find(priority_queue* q, double rating, int* depth) {
    int i, d;
    
    if (q->size - 1 == last(q->height)) {
        i = q->size - 1;
        d = q->height;
    } else {
        i = last(q->height - 1);
        d = q->height - 1;
    }

    while (i < q->size && q->customers[i]->rating != rating) {
        if (rating > q->customers[i]->rating) {
            i = left_parent(i, d);
            d--;
        } else {
            int c_i = left_child(i, d);
            if (c_i >= q->size || (d != 1 && i == last(d))) {
                i--;
            } else {
                i = c_i;
                d++;
            }
        }
    }

    if (i >= q->size) {
        return -1;
    }

    *depth = d;
    return i;
}

priority_queue* make_queue() {
    priority_queue* q = (priority_queue*) xmalloc(sizeof(priority_queue));

    q->size = 0;
    q->height = 0;
    q->ratings = make_map();

    return q;
}

void enqueue(priority_queue* q, const customer* c) {
    map_add(q->ratings, c->name, c->rating);

    int n = q->size;
    q->size++;

    if (q->size == 1) {
        q->customers[n] = c;
        q->height = 1;
        return;
    }

    sift_up(q, c, q->size - 1, q->height);

    if (q->size > last(q->height)) {
        q->height++;
    }
}

customer* dequeue(priority_queue* q) {
    if (q->size == 0) {
        return NULL;
    }

    customer* c = (customer*) q->customers[0];
    delete_at(q, 0, 1);

    return c;
}

customer* cancel(priority_queue* q, const char* name) {
    double rating = map_get(q->ratings, name);

    int depth;
    int index = find(q, rating, &depth);

    assert(index != -1);

    customer* c = (customer*) q->customers[index];

    delete_at(q, index, depth);

    return c;
}

void destroy_queue(priority_queue* q) {
    destroy_map(q->ratings);
    free(q);
}
