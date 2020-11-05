#include "tokens.h"

#include <stdio.h>

int main() {
    for (;;) {
        int m, n,
            lido = scanf("%d %d", &m, &n);
        if (lido == 0 || lido == -1)
            break;

        saco s = { 0 };

        for (int i = 0; i < m; i++) {
            token t;
            t.tipo = TOKEN_SIMPLES;

            scanf("%d", &t.num);
            scanf(" \"%6[^\"]\"", t.msg);

            add_token(&s, t);
        }

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
