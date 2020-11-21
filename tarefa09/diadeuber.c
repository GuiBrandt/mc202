/**
 * @file diadeuber.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação da lógica da tarefa.
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa09.html
 */

#include "priority_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void read_customer(customer* c) {
    scanf("%s ", c->name);
    scanf("%lf ", &c->rating);
    scanf("%d %d ", &c->position.x, &c->position.y);
    scanf("%d %d ", &c->dest.x, &c->dest.y);
}

size_t d1(point a, point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int main() {
    char op;
    priority_queue* q = make_queue();

    size_t total_km = 0;
    double income = 0;
    point driver_pos = { 0, 0 };

    customer* current_customer = NULL;

    while (scanf("%c ", &op) != EOF) {
        if (op == 'A') {
            customer* c = (customer*) malloc(sizeof(customer));
            read_customer(c);

            enqueue(q, c);

            if (current_customer == NULL) {
                current_customer = dequeue(q);
                total_km += d1(driver_pos, current_customer->position);
                driver_pos = current_customer->position;
            }

            printf("Cliente %s foi adicionado(a)\n", c->name);

        } else if (op == 'F') {
            printf("A corrida de %s foi finalizada\n", current_customer->name);

            income += 1.4 * d1(driver_pos, current_customer->dest);
            total_km += d1(driver_pos, current_customer->dest);

            driver_pos = current_customer->dest;

            free(current_customer);

            current_customer = dequeue(q);

            if (current_customer != NULL) {
                total_km += d1(driver_pos, current_customer->position);
                driver_pos = current_customer->position;
            }

        } else if (op == 'C') {
            char name[16];
            scanf("%15s", name);

            customer* c = cancel(q, name);
            printf("%s cancelou a corrida\n", c->name);

            free(c);

            income += 7;

        } else if (op == 'T') {
            break;
        }
    }

    destroy_queue(q);

    printf("\n");
    printf("Jornada finalizada. Aqui esta o seu rendimento de hoje\n");
    printf("Km total: %lu\n", total_km);

    printf("Rendimento bruto: %.2lf\n", income);

    double expenses = 57 + (total_km / 10.0) * 4.104;
    printf("Despesas: %.2lf\n", expenses);

    printf("Rendimento liquido: %.2lf\n", income - expenses - income / 4);

    return 0;
}
