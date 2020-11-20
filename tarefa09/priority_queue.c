/**
 * @file priority_queue.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação de fila de prioridade com Beap [MS80].
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa09.html
 * 
 * [MS80]
 *  Munro, J.I. e Suwanda, H., 1980.
 *  Implicit data structures for fast search and update.
 *  Journal of Computer and System Sciences, 21(2), pp. 236-250.
 * Disponível em:
 *  https://www.sciencedirect.com/science/article/pii/0022000080900379
 */

#include "priority_queue.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <math.h>

struct priority_queue {
    const customer* customers[249];
    size_t size;
    size_t height;
};

priority_queue* make_queue() {
    priority_queue* q = (priority_queue*) malloc(sizeof(priority_queue));

    if (q == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

    q->size = 0;
    q->height = 0;
}

void destroy_queue(priority_queue* q) {
    free(q);
}

int first(int h) {
    return h * (h + 1) / 2;
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

void enqueue(priority_queue* q, const customer* c) {
    int n = q->size;
    q->size++;

    if (q->size == 1) {
        q->customers[n] = c;
        return;
    }

    if (q->size > last(q->height)) {
        q->height++;
    }

    int i = n;
    for (int h = q->height; h > 1; h--) {
        int l_i, r_i;
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

    if (c->rating > q->customers[0]->rating) {
        q->customers[i] = q->customers[0];
        q->customers[0] = c;
    } else {
        q->customers[i] = c;        
    }
}

int main() {
    priority_queue* q = make_queue();

    customer c[12];
    double ratings[] = { 2.1, 7.2, 3.1, 7.3, 7.8, 4.1, 7.7, 18.1, 8.1, 7.6, 9.1, 19.0 };

    for (int i = 0; i < 12; i++) {
        c[i].rating = ratings[i];
        enqueue(q, &c[i]);
    }

    for (int i = 0; i < 12; i++) {
        printf("%lf ", q->customers[i]->rating);
    }

    printf("\n");

    return 0;
}
