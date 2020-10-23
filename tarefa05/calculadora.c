/**
 * @file calculadora.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa05.html 
 */

#include <stdio.h>
#include <assert.h>

#include "bignum.h"

int main() {
    int n;
    scanf("%d ", &n);

    for (int i = 0; i < n; i++) {
        char operator;
        scanf("%c ", &operator);

        bignum lhs, rhs;
        bignum_init(&lhs);
        bignum_init(&rhs);

        char num[25];
        scanf("%s ", num);
        bignum_parse(&lhs, num);

        scanf("%s ", num);
        bignum_parse(&rhs, num);

        switch (operator) {
            case '+':
                bignum_add(&lhs, &rhs);
                break;

            case '-':
                bignum_subtract(&lhs, &rhs);
                break;
            
            case '*':
                bignum_multiply(&lhs, &rhs);
                break;
            
            case '/':
                bignum_divide(&lhs, &rhs);
                break;
            
            default:
                assert(0);
        }

        char result[52];
        bignum_sprintf(result, 52, &lhs);
        printf("%s\n", result);

        bignum_destroy(&lhs);
        bignum_destroy(&rhs);
    }

    return 0;
}
