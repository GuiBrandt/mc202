/**
 * @file caminho.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação da lógica da tarefa.
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa11.html 
 */

#include <stdio.h>

#include <math.h>
#include <string.h>
#include <assert.h>

#include "pokegraph.h"

int main() {
    point_t origin;
    scanf("%lf %lf ", &origin.x, &origin.y);

    graph_t* g = make_graph();

    point_t p;
    while (scanf("%lf ", &p.x) != EOF) {
        char flag[9];
        scanf("%lf %s ", &p.y, flag);

        waypoint_type t;
        if (strcmp(flag, "pokestop") == 0) {
            t = POKESTOP;
        } else {
            assert(strcmp(flag, "Lugia") == 0);
            t = LUGIA;
        }

        add_waypoint(g, p, t);
    }

    double the_very_best = minimum_greatest_interval(g, origin);
    printf("%d", (int) ceil(the_very_best));

    destroy_graph(g);

    return 0;
}