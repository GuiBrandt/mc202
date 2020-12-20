#define _GNU_SOURCE // fmemopen

#include "expressions.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "xmalloc.h"

typedef enum expression_type {
    SIGNED_INT,
    REFERENCE,
    ARITHMETIC
} expression_type;

struct expression {
    expression_type type;

    union {
        int value;
        reference_t reference;

        struct {
            int sign;
            expression_t *left, *right;
        } arithmetic;
    };
};

expression_t* make_int_expr(int value) {
    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = SIGNED_INT;
    expr->value = value;
    return expr;
}

expression_t* make_reference_expr(char column, size_t row) {
    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = REFERENCE;
    expr->reference.col = column;
    expr->reference.row = row;
    return expr;
}

expression_t* make_arithmetic_expr(
    char op,
    expression_t* left,
    expression_t* right
) {
    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = ARITHMETIC;
    expr->arithmetic.sign = op == '+' ? 1 : -1;
    expr->arithmetic.left = left;
    expr->arithmetic.right = right;
    return expr;
}

expression_t* parse_int(FILE* input) {
    int value;
    int read = fscanf(input, " %d ", &value);

    if (read != 1) {
        return NULL;
    }

    return make_int_expr(value);
}

expression_t* parse_reference(FILE* input) {
    char column;
    size_t row;

    int read = fscanf(input, " %[A-Z]%lu ", &column, &row);

    if (read != 2) {
        return NULL;
    }

    return make_reference_expr(column, row);
}

expression_t* parse_arithmetic(FILE* input);

expression_t* parse_parens(FILE* input) {
    char c = fgetc(input);

    if (c != '(') {
        ungetc(c, input);
        return NULL;
    }

    expression_t* expr = parse_arithmetic(input);

    if (expr == NULL) {
        return NULL;
    }

    fscanf(input, " ");
    assert(fgetc(input) == ')');

    return expr;
}

expression_t* parse_value(FILE* input) {
    expression_t* expr = parse_int(input);
    if (expr == NULL) expr = parse_reference(input);
    if (expr == NULL) expr = parse_parens(input);
    return expr;
}

expression_t* parse_arithmetic(FILE* input) {
    expression_t *left, *right;
    
    left = parse_value(input);

    if (left == NULL) {
        return NULL;
    }

    char op;
    int read = fscanf(input, " %c ", &op);

    if (read != 1) {
        return left;
    }

    right = parse_value(input);

    if (right == NULL) {
        destroy_expression(left);
        return NULL;
    }

    return make_arithmetic_expr(op, left, right);
}

expression_t* pure(int value) {
    return make_int_expr(value);
}

expression_t* parse(const char* str) {
    FILE* input = fmemopen((char*) str, strlen(str), "r");

    if (input == NULL) {
        fprintf(stderr, "Erro fatal: fmemopen falhou com código %d", errno);
        exit(-1);
    }

    expression_t* expr;
    
    char c = fgetc(input);
    if (c == '=') {
        expr = parse_arithmetic(input);
    } else {
        ungetc(c, input);
        expr = parse_int(input);
    }
    
    fclose(input);

    return expr;
}

int eval(const expression_t* expr, void* context, resolve_fn_t resolve) {
    assert(expr != NULL);

    switch (expr->type) {
        case SIGNED_INT:
            return expr->value;

        case REFERENCE: {
            const expression_t* referenced = resolve(context, expr->reference);

            if (referenced == NULL) {
                return INT_MIN;
            }

            return eval(referenced, context, resolve);
        }

        case ARITHMETIC: {
            int sign = expr->arithmetic.sign;
            const expression_t* left = expr->arithmetic.left;
            const expression_t* right = expr->arithmetic.right;

            int lvalue = eval(left, context, resolve),
                rvalue = eval(right, context, resolve);

            if (lvalue == INT_MIN || rvalue == INT_MIN) {
                return INT_MIN;
            }

            return lvalue + sign * rvalue;
        }

        default:
            __builtin_unreachable();
    }
}

int dependencies(const expression_t* expr, size_t buf_len, reference_t* buf) {
    assert(expr != NULL);

    switch (expr->type) {
        case REFERENCE: {
            buf[0] = expr->reference;
            return 1;
        }

        case ARITHMETIC: {
            const expression_t* left = expr->arithmetic.left;
            const expression_t* right = expr->arithmetic.right;

            int n = dependencies(left, buf_len, buf);
            n += dependencies(right, buf_len - n, buf + n);

            return n;
        }

        default:
            return 0;
    }
}

void destroy_expression(expression_t* expr) {
    if (expr == NULL) {
        return;
    }

    if (expr->type == ARITHMETIC) {
        destroy_expression(expr->arithmetic.left);
        destroy_expression(expr->arithmetic.right);
    }

    free(expr);
}

void syntax_tree_rec(expression_t* expr, int depth, int nested) {
    switch (expr->type) {
        case SIGNED_INT:
            printf("%d\n", expr->value);
            break;

        case REFERENCE:
            printf("%c%lu\n", expr->reference.col, expr->reference.row);
            break;
        
        case ARITHMETIC:
            printf("%c\n", expr->arithmetic.sign == 1 ? '+' : '-');

            for (int i = 0; i < nested; i++) printf("│ ");
            for (int i = nested; i < depth; i++) printf("  ");
            printf("├ ");
            syntax_tree_rec(expr->arithmetic.left, depth + 1, nested + 1);
            
            for (int i = 0; i < nested; i++) printf("│ ");
            for (int i = nested; i < depth; i++) printf("  ");
            printf("└ ");
            syntax_tree_rec(expr->arithmetic.right, depth + 1, nested);

            break;

        default:
            __builtin_unreachable();
    }
}

void syntax_tree(expression_t* expr) {
    if (expr == NULL) {
        printf("(NULL)\n");
    } else {
        syntax_tree_rec(expr, 0, 0);
    }
}

const expression_t* resolve_simple(void* context, char column, size_t row) {
    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = SIGNED_INT;
    expr->value = (int) row;
    return expr;
}
