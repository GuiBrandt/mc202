/**
 * @file listas_legais.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação da lógica da tarefa.
 *
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa08.html
 */

#include "tree_multiset.h"

#include <stdio.h>
#include <inttypes.h>

int main() {
    tree_multiset* s = multiset_init();

    // Leitura dos elementos iniciais do conjunto
    int n, k;
    scanf("%d %d\n", &n, &k);

    for (int i = 0; i < n; i++) {
        element_t v;
        scanf("%"SCNu64" ", &v);

        multiset_insert(s, v);
    }

    // Processamento das operações
    for (int i = 0; i < k; i++) {
        int op;
        scanf("%d ", &op);

        // Inserção
        if (op == 1) {
            element_t j;
            scanf("%"SCNu64" ", &j);

            multiset_insert(s, j);

        // Contagem
        } else if (op == 2) {
            element_t m;
            scanf("%"SCNu64" ", &m);

            element_t c = multiset_count(s, m);
            printf("%"PRIu64"\n", c);

        // "Diferença-legal"
        } else {
            printf("%"PRIu64"\n", multiset_diff_cool(s));
        }
    }

    multiset_destroy(s);

    return 0;
}