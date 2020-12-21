/**
 * @file expressions.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de expressão de célula da planilha.
 */

#define _GNU_SOURCE // Para o fmemopen

#include "expressions.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "xmalloc.h"

//=============================================================================
// DEFINIÇÕES
//=============================================================================

/**
 * @brief Enumeração de tipos de expressão.
 */
typedef enum expression_type {
    SIGNED_INT,
    REFERENCE,
    ARITHMETIC
} expression_type;

/**
 * @brief Estrutura interna de uma expressão.
 */
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

//=============================================================================
// IMPLEMENTAÇÃO (Auxiliares)
//=============================================================================

/**
 * @brief Constrói uma expressão numérica com um valor dado.
 * 
 * @param value valor da expressão.
 * 
 * @return uma expressão.
 */
expression_t* make_int_expr(int value) {
    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = SIGNED_INT;
    expr->value = value;
    return expr;
}

/**
 * @brief Constrói uma expressão de referência para uma célula na planilha.
 * 
 * @param column nome da coluna (A-Z).
 * @param row número da linha.
 * 
 * @return uma expressão.
 */
expression_t* make_reference_expr(char column, size_t row) {
    expression_t* expr = (expression_t*) xmalloc(sizeof(expression_t));
    expr->type = REFERENCE;
    expr->reference.col = column;
    expr->reference.row = row;
    return expr;
}

/**
 * @brief Constrói uma expressão aritmética para um operador e operandos dados.
 * 
 * @param op operador, "+" ou "-".
 * @param left operando esquerdo.
 * @param right operando direito.
 * 
 * @return uma expressão.
 */
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

//=============================================================================
// IMPLEMENTAÇÃO (Parser)
//=============================================================================

/**
 * @brief Lê uma expressão numérica de uma stream.
 * 
 * @param input stream de entrada.
 * 
 * @return a expressão lida ou NULL em caso de falha.
 */
expression_t* parse_int(FILE* input) {
    int value;
    int read = fscanf(input, " %d ", &value);

    if (read != 1) {
        return NULL;
    }

    return make_int_expr(value);
}

/**
 * @brief Lê uma expressão de referência de uma stream.
 * 
 * @param input stream de entrada.
 * 
 * @return a expressão de lida ou NULL em caso de falha.
 */
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

/**
 * @brief Lê uma expressão entre parênteses de uma stream.
 * 
 * @param input stream de entrada.
 * 
 * @return a expressão lida ou NULL em caso de falha.
 */
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

/**
 * @brief Lê uma expressão genérica (numérica, de referência ou entre
 *        parênteses).
 * 
 * @param input stream de entrada.
 * 
 * @return a expressão lida ou NULL em caso de falha.
 */
expression_t* parse_value(FILE* input) {
    expression_t* expr = parse_int(input);
    if (expr == NULL) expr = parse_reference(input);
    if (expr == NULL) expr = parse_parens(input);
    return expr;
}

/**
 * @brief Lê uma expressão aritmética de uma stream.
 * 
 * @param input stream de entrada.
 * 
 * @return a expressão lida ou NULL em caso de falha.
 */
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
        destroy_expr(left);
        return NULL;
    }

    return make_arithmetic_expr(op, left, right);
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

expression_t* const_expr(int value) {
    return make_int_expr(value);
}

expression_t* parse_expr(const char* str) {
    // Ao invés de usar a string diretamente, usamos ela como uma stream.
    // Isso é conveniente porque permite usar as funções de scan da stdlib
    // junto do mecanismo automatico de deslocamento do ponteiro de leitura,
    // que não seria possível com sscanf por exemplo. Também permite que as
    // funções do parser aceitem arquivos como argumento e sejam mais flexíveis
    // em relação a I/O.
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

int eval_expr(const expression_t* expr, void* context, resolve_fn_t resolve) {
    assert(expr != NULL);

    switch (expr->type) {
        case SIGNED_INT:
            return expr->value;

        case REFERENCE: {
            const expression_t* referenced = resolve(context, expr->reference);

            if (referenced == NULL) {
                return INT_MIN;
            }

            return eval_expr(referenced, context, resolve);
        }

        case ARITHMETIC: {
            int sign = expr->arithmetic.sign;
            const expression_t* left = expr->arithmetic.left;
            const expression_t* right = expr->arithmetic.right;

            int lvalue = eval_expr(left, context, resolve),
                rvalue = eval_expr(right, context, resolve);

            if (lvalue == INT_MIN || rvalue == INT_MIN) {
                return INT_MIN;
            }

            return lvalue + sign * rvalue;
        }

        default:
            __builtin_unreachable();
    }
}

int expr_dependencies(
    const expression_t* expr,
    size_t buf_len,
    reference_t* buf
) {
    assert(buf_len > 0);
    assert(expr != NULL);

    switch (expr->type) {
        case REFERENCE: {
            buf[0] = expr->reference;
            return 1;
        }

        case ARITHMETIC: {
            const expression_t* left = expr->arithmetic.left;
            const expression_t* right = expr->arithmetic.right;

            int n = expr_dependencies(left, buf_len, buf);
            n += expr_dependencies(right, buf_len - n, buf + n);

            return n;
        }

        default:
            return 0;
    }
}

void destroy_expr(expression_t* expr) {
    if (expr == NULL) {
        return;
    }

    if (expr->type == ARITHMETIC) {
        destroy_expr(expr->arithmetic.left);
        destroy_expr(expr->arithmetic.right);
    }

    free(expr);
}
