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
#include <stdbool.h>

#include <math.h>

#include "xmalloc.h"

//=============================================================================
// Funções Auxiliares
//=============================================================================

/**
 * @brief Aloca memória e lê as informações de um cliente da entrada padrão.
 * 
 * @return um ponteiro para o cliente alocado e lido.
 */
customer* read_customer() {
    customer* c = (customer*) xmalloc(sizeof(customer));

    scanf("%s ", c->name);
    scanf("%lf ", &c->rating);
    scanf("%d %d ", &c->position.x, &c->position.y);
    scanf("%d %d ", &c->dest.x, &c->dest.y);

    return c;
}

/**
 * @brief Calcula a distância de manhattan (L1) entre dois pontos.
 * 
 * @param a um ponto cartesiano.
 * @param b outro ponto cartesiano.
 * 
 * @return a distância l1
 */
size_t l1(point a, point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

//=============================================================================
// Implementação (Motorista)
//=============================================================================

/**
 * @brief Estrutura com as informações relevantes ao motorista.
 * 
 */
typedef struct driver {
    priority_queue* queue;
    customer* current_customer;
    point pos;
    struct stats {
        size_t total_km;
        double income;
    } stats;
} driver;

/**
 * @brief Inicializa uma estrutura de motorista.
 * 
 * @param driver ponteiro para o motorista.
 */
void init_driver(driver* driver) {
    driver->queue = make_queue();
    driver->stats.total_km = 0;
    driver->stats.income = 0;
    driver->pos = (point) { 0, 0 };
    driver->current_customer = NULL;
}

/**
 * @brief Libera os recursos alocados em uma estrutura de motorista.
 * 
 * @param driver ponteiro para o motorista.
 */
void destroy_driver(driver* driver) {
    destroy_queue(driver->queue);
}

/**
 * @brief Determina se o motorista pode atender o próximo cliente.
 * 
 * @param driver ponteiro para o motorista.
 * 
 * @return verdadeiro se o motorista não estiver no meio de uma corrida, falso
 *         se não.
 */
bool is_available(const driver* driver) {
    return driver->current_customer == NULL;
}

/**
 * @brief Procedimento de pegar o próximo cliente para a corrida.
 * 
 * Caso a fila esteja vazia, esta função marca o motorista como disponível.
 * 
 * @param driver ponteiro para o motorista.
 */
void pickup_next_customer(driver* driver) {
    driver->current_customer = dequeue(driver->queue);

    if (driver->current_customer == NULL) {
        return;
    }

    point customer_pos = driver->current_customer->position;
    driver->stats.total_km += l1(driver->pos, customer_pos);
    driver->pos = customer_pos;
}

/**
 * @brief Exibe as estatísticas do dia para o motorista.
 * 
 * @param driver ponteiro para o motorista.
 */
void print_stats(driver* driver) {
    printf("\n");
    printf("Jornada finalizada. Aqui esta o seu rendimento de hoje\n");
    printf("Km total: %lu\n", driver->stats.total_km);
    printf("Rendimento bruto: %.2lf\n", driver->stats.income);

    double expenses = 57 + (driver->stats.total_km / 10.0) * 4.104;
    printf("Despesas: %.2lf\n", expenses);

    double net_yield = driver->stats.income * 3 / 4 - expenses;
    printf("Rendimento liquido: %.2lf\n", net_yield);
}

//=============================================================================
// IMPLEMENTAÇÃO (Comandos)
//=============================================================================

void accept_customer(driver* driver) {
    customer* c = read_customer();

    enqueue(driver->queue, c);

    printf("Cliente %s foi adicionado(a)\n", c->name);

    if (is_available(driver)) {
        pickup_next_customer(driver);
    }
}

void finish_ride(driver* driver) {
    customer* customer = driver->current_customer;

    printf("A corrida de %s foi finalizada\n", customer->name);

    driver->stats.income += 1.4 * l1(driver->pos, customer->dest);
    driver->stats.total_km += l1(driver->pos, customer->dest);

    driver->pos = customer->dest;

    free(customer);

    pickup_next_customer(driver);
}

void cancel_request(driver* driver) {
    char name[16];
    scanf("%15s", name);

    customer* c = cancel(driver->queue, name);
    printf("%s cancelou a corrida\n", c->name);

    free(c);

    driver->stats.income += 7;
}

void terminate(driver* driver) {
    print_stats(driver);
    destroy_driver(driver);
}

//=============================================================================
// PONTO DE ENTRADA
//=============================================================================

int main() {
    char op;

    driver driver;
    init_driver(&driver);

    while (scanf("%c ", &op) != EOF) {
        switch (op) {
            case 'A':
                accept_customer(&driver);
                break;

            case 'F':
                finish_ride(&driver);
                break;

            case 'C':
                cancel_request(&driver);
                break;

            case 'T':
                terminate(&driver);
                return 0;
        }
    }

    // Toda sequência de operações deve terminar com 'T'
    __builtin_unreachable();

    return 1;
}
