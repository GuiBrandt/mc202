#ifndef __EXPRESSIONS_H__
#define __EXPRESSIONS_H__

#include <stddef.h>

typedef struct reference {
    char col;
    size_t row;
} reference_t;

typedef struct expression expression_t;

typedef const expression_t* (*resolve_fn_t)(void* ctx, reference_t ref);

expression_t* pure(int value);
expression_t* parse(const char* str);
int eval(const expression_t* expr, void* context, resolve_fn_t resolve);
int dependencies(const expression_t* expr, size_t buf_len, reference_t* buf);
void destroy_expression(expression_t* expr);

#endif // __EXPRESSIONS_H__
