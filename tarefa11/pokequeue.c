#include "pokequeue.h"

#include <assert.h>
#include <stdlib.h>

#include <stdio.h>

#include "xmalloc.h"

struct queue {
    size_t bitmask;
    size_t left, right;
    element_t* elements;
};

inline extern size_t next_pow2(size_t n) {
    return n <= 1 ? 1 : 1 << (64 - __builtin_clzl(n - 1));
}

queue_t* make_queue(size_t capacity) {
    queue_t* queue = (queue_t*) xmalloc(sizeof(queue_t));

    size_t actual_capacity = next_pow2(capacity);
    queue->bitmask = actual_capacity - 1;
    queue->left = queue->right = 0;
    queue->elements = (element_t*) xmalloc(actual_capacity * sizeof(element_t));
    
    return queue;
}

void enqueue(queue_t* queue, element_t element) {
    queue->elements[queue->right] = element;
    queue->right = (queue->right + 1) & queue->bitmask;
    
    assert(!queue_empty(queue));
}

element_t dequeue(queue_t* queue) {
    assert(!queue_empty(queue));

    int e = queue->elements[queue->left];
    queue->left = (queue->left + 1) & queue->bitmask;

    return e;
}

bool queue_empty(queue_t* queue) {
    return queue->left == queue->right;
}

void destroy_queue(queue_t* queue) {
    free(queue->elements);
    free(queue);
}
