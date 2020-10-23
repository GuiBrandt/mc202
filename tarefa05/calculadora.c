/**
 * @file calculadora.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa05.html 
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bignum.h"

inline static void expect(result_code code);

//=============================================================================
// Ponto de entrada
//=============================================================================

int main() {
    int n;
    scanf("%d ", &n);

    for (int i = 0; i < n; i++) {
        char operator;
        scanf("%c ", &operator);

        bignum lhs, rhs;
        expect(bignum_init(&lhs));
        expect(bignum_init(&rhs));

        char num[25];
        scanf("%s ", num);
        expect(bignum_parse(&lhs, num));

        scanf("%s ", num);
        expect(bignum_parse(&rhs, num));

        switch (operator) {
            case '+':
                expect(bignum_add(&lhs, &rhs));
                break;

            case '-':
                expect(bignum_subtract(&lhs, &rhs));
                break;
            
            case '*':
                expect(bignum_multiply(&lhs, &rhs));
                break;
            
            case '/':
                expect(bignum_divide(&lhs, &rhs));
                break;
            
            default:
                assert(0);
        }

        char result[52];
        expect(bignum_sprintf(result, 52, &lhs));
        printf("%s\n", result);

        bignum_destroy(&lhs);
        bignum_destroy(&rhs);
    }

    return 0;
}

inline static void expect(result_code code) {
    if (code != SUCCESS) {
        static const char* err[] = {
            [FAIL_OOM] = "Out of memory",
            [FAIL_DIVIDE_BY_ZERO] = "Divisão por zero",
            [FAIL_STRING_OVERFLOW] = "String overflow"
        };

        fprintf(stderr, "Erro fatal: %s\n", err[code]);
        exit(-1);
    }
}
