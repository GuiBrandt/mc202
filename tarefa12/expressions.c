#include "expressions.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>

#include "xmalloc.h"

typedef enum expression_type {
    VALUE,
    REFERENCE,
    ARITHMETIC
} expression_type;

struct expression {
    expression_type type;

    union {
        int value;
        
        struct {
            char column;
            size_t row;
        } reference;

        struct {
            int sign;
            expression_t *left, *right;
        } arithmetic;
    };
};

expression_t* parse_int(FILE* input) {
    int value;
    int read = fscanf(input, " %d ", &value);

    if (read != 1) {
        return NULL;
    }

    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = VALUE;
    expr->value = value;
    return expr;
}

expression_t* parse_reference(FILE* input) {
    char column;
    size_t row;

    int read = fscanf(input, " %[A-Z]%lu ", &column, &row);

    if (read != 2) {
        return NULL;
    }

    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = REFERENCE;
    expr->reference.column = column;
    expr->reference.row = row;
    return expr;
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
    long pos = ftell(input);
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

    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = ARITHMETIC;
    expr->arithmetic.sign = op == '+' ? 1 : -1;
    expr->arithmetic.left = left;
    expr->arithmetic.right = right;
    return expr;
}

expression_t* parse(const char* str) {
    FILE* input = fmemopen((char*) str, strlen(str), "r");
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

int eval(const expression_t* expr, resolve_fn_t resolve) {

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
        case VALUE:
            printf("%d\n", expr->value);
            break;

        case REFERENCE:
            printf("%c%lu\n", expr->reference.column, expr->reference.row);
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

int main() {
    expression_t* expr = parse("= ( 1 - ( A1 + ( B2 - C4 ) ) ) + 1");
    syntax_tree(expr);
    destroy_expression(expr);
    return 0;
}
