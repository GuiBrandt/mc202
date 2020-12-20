/**
 * @file xmalloc.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação da função malloc adaptada para tratar OOM.
 */

#include "xmalloc.h"

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
