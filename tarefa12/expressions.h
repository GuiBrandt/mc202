#ifndef __EXPRESSIONS_H__
#define __EXPRESSIONS_H__

#include <stddef.h>

typedef struct expression expression_t;

typedef const expression_t* (*resolve_fn_t)(char column, size_t row);

expression_t* parse(const char* str);
int eval(const expression_t* expr, resolve_fn_t resolve);
void destroy_expression(expression_t* expr);

#endif // __EXPRESSIONS_H__
