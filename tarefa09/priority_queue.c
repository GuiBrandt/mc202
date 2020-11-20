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
#include "util.h"

struct priority_queue {
    const customer* customers[249];
    map* ratings;
    size_t size;
    size_t height;
};

int first(int h) {
    return h * (h - 1) / 2;
}

int last(int h) {
    return first(h + 1) - 1;
}

int left_parent(int i, int h) {
    return i - h;
}

int right_parent(int i, int h) {
    return i - h + 1;
}

int left_child(int i, int h) {
    return i + h;
}

int right_child(int i, int h) {
    return i + h + 1;
}

int sift_up(priority_queue* q, const customer* c, int i, int h) {
    for (; h > 1; h--) {
        int l_i = -1, r_i = -1;
        double l_rating, r_rating;
        
        if (i != first(h)) {
            l_i = left_parent(i, h);
            l_rating = q->customers[l_i]->rating;
        } else {
            l_rating = INFINITY;
        }

        if (i != last(h)) {
            r_i = right_parent(i, h);
            r_rating = q->customers[r_i]->rating;
        } else {
            r_rating = INFINITY;
        }

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
        } else {
            q->customers[i] = c;
            return h;
        }
    }

    if (c->rating > q->customers[0]->rating) {
        q->customers[i] = q->customers[0];
        q->customers[0] = c;
    } else {
        q->customers[i] = c;
    }

    return h;
}

void sift_down(priority_queue* q, const customer* c, int i, int h) {
    for (; h <= q->height; h++) {
        int l_i = left_child(i, h), r_i;
        double l_rating, r_rating;
        
        if (l_i < q->size) {
            l_rating = q->customers[l_i]->rating;

            r_i = right_child(i, h);

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

void delete_at(priority_queue* q, int i, int h) {
    q->size--;
    const customer* c = q->customers[q->size];

    // No máximo um dos dois tem efeito, mas não tem vantagem verificar antes
    // qual deve acontecer (a lógica seria a mesma da função).
    sift_up(q, c, i, h);
    sift_down(q, c, i, h);
    
    if (q->size < first(q->height)) {
        q->height--;
    }
}

int find(priority_queue* q, double rating, int* height) {
    int i, h;
    if (q->size - 1 == last(q->height)) {
        i = q->size - 1;
        h = q->height;
    } else {
        i = last(q->height - 1);
        h = q->height - 1;
    }

    while (i < q->size && q->customers[i]->rating != rating) {
        if (rating > q->customers[i]->rating) {
            i = left_parent(i, h);
            h--;
        } else {
            int c_i = left_child(i, h);
            if (c_i >= q->size || (h != 1 && i == last(h))) {
                i--;
            } else {
                i = c_i;
                h++;
            }
        }
    }

    if (i >= q->size) {
        return -1;
    }

    *height = h;
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

    int height;
    int index = find(q, rating, &height);

    assert(index != -1);

    customer* c = (customer*) q->customers[index];

    delete_at(q, index, height);

    return c;
}

void destroy_queue(priority_queue* q) {
    destroy_map(q->ratings);
    free(q);
}
