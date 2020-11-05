/**
 * @file mensageiro.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definições do ADT para os tokens do mensageiro.
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa07.html
 */

#include "tokens.h"

#include <stdio.h>

//=============================================================================
// Ponto de entrada
//=============================================================================

int main() {
    for (;;) {
        int m, n;
        
        int lido = scanf("%d %d", &m, &n);
        if (lido == 0 || lido == -1)
            break;

        saco s = make_saco();

        // Cartões
        for (int i = 0; i < m; i++) {
            int num;
            scanf("%d", &num);

            char msg[TOKEN_MSG_MAXLEN + 1];
            scanf(" \"%6[^\"]\"", msg);

            add_token(&s, num, msg);
        }

        // Autoridades
        for (int i = 0; i < n; i++) {
            int x;
            scanf("%d", &x);
            troca_triade(&s, x);
        }

        print_mensagem(&s);
        printf("\n");

        destroy(&s);
    }

    return 0;
}
