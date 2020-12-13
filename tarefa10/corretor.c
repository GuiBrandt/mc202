/**
 * @file corretor.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação da lógica da tarefa.
 *
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa10.html 
 */

#include <stdio.h>

#include "dictionary.h"

int main() {
    int n, q;
    scanf("%d %d ", &n, &q);


    dict* d = make_dict();

    char word[27];
    for (int i = 0; i < n; i++) {
        scanf("%s ", word);
        dict_insert(d, word);
    }

    for (int i = 0; i < q; i++) {
        scanf("%s ", word);
        color c = dict_query(d, word);

        switch (c) {
            case GREEN: printf("verde\n"); break;
            case YELLOW: printf("amarelo\n"); break;
            case RED: printf("vermelho\n"); break;
            default: __builtin_unreachable();
        }
    }

    destroy_dict(d);

    return 0;
}
