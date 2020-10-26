#include "deque.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

deque_node_t* make_node(deque_elem_t elem, deque_node_t* prev, deque_node_t* next) {
    deque_node_t* node = (deque_node_t*) malloc(sizeof(deque_node_t));

    if (node == NULL) {
        fprintf(stderr, "Out of memory");
        exit(-1);
    }

    node->elem = elem;
    node->prev = prev;
    node->next = next;

    return node;
}

deque make_deque() {
    return (deque) { .head = NULL, .tail = NULL };
}

void destroy_deque(deque* q) {
    assert(q != NULL);

    deque_node_t* it = q->head;

    while (it != NULL) {
        deque_node_t* aux = it;
        it = it->next;
        free(aux);
    }
}

int is_empty(const deque* q) {
    assert(q != NULL);

    return q->head == NULL;
}

void push_back(deque* q, deque_elem_t elem) {
    assert(q != NULL);

    deque_node_t* node = make_node(elem, q->tail, NULL);

    if (q->tail == NULL) {
        q->head = q->tail = node;
    } else {
        q->tail->next = node;
        q->tail = node;
    }
}

void push_front(deque* q, deque_elem_t elem) {
    assert(q != NULL);

    deque_node_t* node = make_node(elem, NULL, q->head);

    if (q->head == NULL) {
        q->head = q->tail = node;
    } else {
        q->head->prev = node;
        q->head = node;
    }
}

deque_elem_t front(const deque* q) {
    assert(q != NULL);
    assert(q->head != NULL);

    return q->head->elem;
}

deque_elem_t back(const deque* q) {
    assert(q != NULL);
    assert(q->tail != NULL);

    return q->tail->elem;
}

void pop_back(deque* q) {
    assert(q != NULL);
    assert(q->tail != NULL);

    deque_node_t* tail = q->tail;
    q->tail = tail->prev;
 
    if (q->tail == NULL) { // Fila ficou vazia
        q->head = NULL;

    } else {
        q->tail->next = NULL;
    }

    free(tail);
}

void pop_front(deque* q) {
    assert(q != NULL);
    assert(q->head != NULL);

    deque_node_t* head = q->head;
    q->head = head->next;

    if (q->head == NULL) { // Fila ficou vazia
        q->tail = NULL;
    } else {
        q->head->prev = NULL;
    }

    free(head);
}
