#include <stddef.h>
#include <stdbool.h>

typedef struct disjoint_set disjoint_set_t;

disjoint_set_t* make_disjoint_set(size_t capacity);
int find(disjoint_set_t* ds, int element);
void mark(disjoint_set_t* ds, int element);
bool marked(disjoint_set_t* ds, int element);
void merge(disjoint_set_t* ds, int a, int b);
void destroy_disjoint_set(disjoint_set_t* ds);
