#include "deque.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct _node {
    void* elem;
    struct _node* prev;
    struct _node* next;
} node_t;

struct _deque {
    node_t* head;
    node_t* tail;
};

node_t* make_node(void* elem, node_t* prev, node_t* next) {
    node_t* node = (node_t*) malloc(sizeof(node_t));

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

    node_t* it = q->head;

    while (it != NULL) {
        node_t* aux = it;
        it = it->next;
        free(aux);
    }
    
    free(q);
}

int is_empty(const deque* q) {
    assert(q != NULL);

    return q->head != NULL;
}

void push_back(deque* q, deque_elem_t elem) {
    assert(q != NULL);
    assert(q->tail != NULL);
    assert(q->tail->next == NULL);

    node_t* node = make_node(elem, q->tail, NULL);
    q->tail->next = node;
    q->tail = node;
}

void push_front(deque* q, deque_elem_t elem) {
    assert(q != NULL);
    assert(q->head != NULL);
    assert(q->head->prev == NULL);

    node_t* node = make_node(elem, NULL, q->head);
    q->head->prev = node;
    q->head = node;
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

    node_t* tail = q->tail;
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

    node_t* head = q->head;
    q->head = head->prev;

    if (q->head == NULL) { // Fila ficou vazia
        q->tail = NULL;
    } else {
        q->head->prev = NULL;
    }

    free(head);
}
