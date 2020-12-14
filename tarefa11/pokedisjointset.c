#include "pokedisjointset.h"

#include <stdlib.h>
#include <stdbool.h>

#include "xmalloc.h"

struct disjoint_set {
    int* parents;
    size_t* sizes;
    bool* marks;
};

disjoint_set_t* make_disjoint_set(size_t capacity) {
    disjoint_set_t* ds = (disjoint_set_t*) xmalloc(sizeof(disjoint_set_t));
    
    ds->parents = (int*) xmalloc(sizeof(int) * capacity);
    ds->sizes = (size_t*) xmalloc(sizeof(size_t) * capacity);
    ds->marks = (bool*) xmalloc(sizeof(bool) * capacity);

    for (int i = 0; i < capacity; i++) {
        ds->parents[i] = i;
        ds->sizes[i] = 1;
        ds->marks[i] = false;
    }

    return ds;
}

int find(disjoint_set_t* ds, int element) {
    int current = element;
    
    while (ds->parents[current] != current) {
        ds->parents[current] = ds->parents[ds->parents[current]];
        current = ds->parents[current];
    }

    return current;
}

void mark(disjoint_set_t* ds, int element) {
    ds->marks[find(ds, element)] = true;
}

bool marked(disjoint_set_t* ds, int element) {
    return ds->marks[find(ds, element)];
}

void merge(disjoint_set_t* ds, int a, int b) {
    int root_a = find(ds, a),
        root_b = find(ds, b);

    if (root_a == root_b) {
        return;
    }

    if (ds->sizes[root_b] > ds->sizes[root_a]) {
        root_a ^= root_b;
        root_b ^= root_a;
        root_a ^= root_b;
    }

    ds->parents[root_b] = root_a;
    ds->sizes[root_a] += ds->sizes[root_b];
    ds->marks[root_a] |= ds->marks[root_b];
}

void destroy_disjoint_set(disjoint_set_t* ds) {
    free(ds->parents);
    free(ds);
}