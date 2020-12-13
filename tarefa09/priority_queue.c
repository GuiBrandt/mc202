/**
 * @file priority_queue.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação de fila de prioridade com remoção com Heap binário.
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa09.html
 */

#include "priority_queue.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>
#include <math.h>

#include "map.h"
#include "xmalloc.h"

//=============================================================================
// Heap binário
//=============================================================================

struct priority_queue {
    const customer* customers[249];
    map* index;
    size_t size;
};

/**
 * @brief Calcula o índice do filho esquerdo de um elemento no Heap.
 * 
 * @param i índice do elemento.
 * 
 * @return índice do filho esquerdo.
 */
inline extern int left_child(int i) {
    return 2 * i + 1;
}

/**
 * @brief Calcula o índice do filho direito de um elemento no Heap.
 * 
 * @param i índice do elemento.
 * 
 * @return índice do filho direito.
 */
inline extern int right_child(int i) {
    return left_child(i) + 1;
}

/**
 * @brief Calcula o índice do pai de um elemento no Heap.
 * 
 * @param i índice do elemento.
 * 
 * @return índice do pai.
 */
inline extern int parent(int i) {
    return (i - 1) / 2;
}

/**
 * @brief Sobe um elemento no Heap até que esteja na posição apropriada.
 * 
 * Complexidade de tempo: O(lg n).
 * 
 * @param q o Heap.
 * @param c o cliente sendo movido.
 * @param i o índice do elemento no Heap.
 */
void sift_up(priority_queue* q, const customer* c, int i) {
    for (
        int p = parent(i);
        p != i && c->rating > q->customers[p]->rating;
        p = parent(i)
    ) {
        map_set(q->index, q->customers[p]->name, i);
        q->customers[i] = q->customers[p];
        i = p;
    }

    map_set(q->index, c->name, i);
    q->customers[i] = c;
}

/**
 * @brief Desce um elemento no Heap até que esteja na posição apropriada.
 * 
 * Complexidade de tempo: O(lg n).
 * 
 * @param q o Heap.
 * @param c o cliente sendo movido.
 * @param i o índice do elemento no Heap.
 */
void sift_down(priority_queue* q, const customer* c, int i) {
    for (int j; i < q->size; i = j) {
        int l_i = left_child(i), r_i = right_child(i);
        double l, r;

        if (l_i < q->size) {
            l = q->customers[l_i]->rating;
        } else {
            l = -INFINITY;
        }

        if (r_i < q->size) {
            r = q->customers[r_i]->rating;
        } else {
            r = -INFINITY;
        }

        if (l > r && l > c->rating) {
            j = l_i;
        } else if (r > c->rating) {
            j = r_i;
        } else {
            break;
        }

        q->customers[i] = q->customers[j];
        map_set(q->index, q->customers[j]->name, i);

        i = j;
    }

    q->customers[i] = c;
    map_set(q->index, c->name, i);
}

/**
 * @brief Remove um elemento do Heap dado seu índice.
 * 
 * Complexidade de tempo: O(lg n).
 * 
 * @param q o Heap.
 * @param i o índice do elemento no Heap.
 */
void delete_at(priority_queue* q, int i) {
    assert(i < q->size);

    q->size--;

    const customer* c = q->customers[i] = q->customers[q->size];

    // No máximo um dos dois tem efeito, mas não tem vantagem verificar antes
    // qual deve acontecer (a lógica seria a mesma da função).
    sift_up(q, c, i);
    sift_down(q, c, i);
}

/**
 * @brief Encontra o índice do elemento com nome dado no Heap.
 * 
 * Complexidade de tempo: O(1).
 * 
 * @param q o Heap.
 * @param name nome do elemento.
 * 
 * @return índice do elemento no Heap.
 */
int find(const priority_queue* q, const char* name) {
    return map_get(q->index, name);
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

priority_queue* make_queue() {
    priority_queue* q = (priority_queue*) xmalloc(sizeof(priority_queue));
    q->size = 0;
    q->index = make_map();
    return q;
}

/**
 * Complexidade de tempo: O(lg n).
 */
void enqueue(priority_queue* q, const customer* c) {
    q->size++;

    if (q->size == 1) {
        map_set(q->index, c->name, 0);
        q->customers[0] = c;
        return;
    }

    sift_up(q, c, q->size - 1);
}

/**
 * Complexidade de tempo: O(lg n).
 */
customer* dequeue(priority_queue* q) {
    if (q->size == 0) {
        return NULL;
    }

    customer* c = (customer*) q->customers[0];
    delete_at(q, 0);

    return c;
}

/**
 * Complexidade de tempo: O(lg n).
 * 
 * Assim como as duas outras operações, só precisamos fazer uma troca (tempo
 * constante) e "Heapify", que toma tempo O(lg n).
 * 
 * Isso só se aplica se tivermos o índice do elemento em tempo O(lg n), no
 * entanto.
 * 
 * Para isso, usa-se um dicionário com tempo de leitura constante¹. Dessa
 * forma, podemos buscar a prioridade para um nome dado e usá-la para buscar
 * pelo elemento no Heap em tempo O(1)¹, mantendo a complexidade da operação
 * como um todo.
 * 
 * ¹ Mais especificamente, com tempo O(k), onde k é o tamanho do nome;
 *   no caso da tarefa, k <= 16, então é seguro dizer que o tempo é de fato
 *   constante (e relativamente pequeno).
 */
customer* cancel(priority_queue* q, const char* name) {
    int index = find(q, name);
    assert(0 <= index && index < q->size);

    customer* c = (customer*) q->customers[index];
    assert(strcmp(c->name, name) == 0);

    delete_at(q, index);

    return c;
}

void destroy_queue(priority_queue* q) {
    destroy_map(q->index);
    free(q);
}
