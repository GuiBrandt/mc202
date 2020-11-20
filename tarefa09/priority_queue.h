/**
 * @file priority_queue.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definições do ADT de fila de prioridade.
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa09.html
 */

/**
 * @brief Estrutura opaca de fila de prioridade. 
 */
typedef struct priority_queue priority_queue;

/**
 * @brief Estrutura de ponto no plano cartesiano.
 */
typedef struct point {
    int x, y;
} point;

/**
 * @brief Estrutura de um cliente na fila.
 */
typedef struct customer {
    char name[16];
    double rating;
    point position;
    point dest;
} customer;

priority_queue* make_queue();

void destroy_queue();

/**
 * @brief Enfileira um cliente.
 * 
 * @param q a fila.
 * @param c o cliente.
 */
void enqueue(priority_queue* q, const customer* c);

/**
 * @brief Enfileira um cliente.
 * 
 * @param q a fila.
 * @param c o cliente.
 */
const customer* pop_first(priority_queue* q);

/**
 * @brief Enfileira um cliente.
 * 
 * @param q a fila.
 * @param c o cliente.
 */
const customer* cancel(priority_queue* q, const char* name);
