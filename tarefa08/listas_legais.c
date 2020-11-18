#include "tree_multiset.h"

#include <stdio.h>
#include <inttypes.h>

int main() {
    tree_multiset* s = multiset_init();

    int n, k;
    scanf("%d %d\n", &n, &k);

    for (int i = 0; i < n; i++) {
        element_t v;
        scanf("%"SCNu64" ", &v);

        multiset_insert(s, v);
    }

    for (int i = 0; i < k; i++) {
        int op;
        scanf("%d ", &op);

        if (op == 1) {
            element_t j;
            scanf("%"SCNu64" ", &j);

            multiset_insert(s, j);

        } else if (op == 2) {
            element_t m;
            scanf("%"SCNu64" ", &m);

            element_t c = multiset_count(s, m);
            printf("%"PRIu64"\n", c);

        } else {
            printf("%"PRIu64"\n", multiset_diff_cool(s));
        }
    }

    multiset_destroy(s);

    return 0;
}