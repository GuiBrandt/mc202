#include "util.h"

#include <stdlib.h>
#include <stdio.h>

void* xmalloc(size_t size) {
    void* ptr = malloc(size);

    if (ptr == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

    return ptr;
}
