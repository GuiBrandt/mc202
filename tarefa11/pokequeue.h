#include <stddef.h>
#include <stdbool.h>

typedef int element_t;
typedef struct queue queue_t;

queue_t* make_queue(size_t capacity);
void enqueue(queue_t* queue, element_t element);
element_t dequeue(queue_t* queue);
bool queue_empty(queue_t* queue);
void destroy_queue(queue_t* queue);
