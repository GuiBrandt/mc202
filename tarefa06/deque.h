/**
 * @file deque.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definições do ADT de Deque.
 */

#ifndef __DEQUE_H__
#define __DEQUE_H__

/**
 * @brief Tipo para elementos de um deque.
 */
typedef void* deque_elem_t;

/**
 * @brief Estrutura de nó do deque. 
 */
typedef struct _node {
    void* elem;
    struct _node* prev;
    struct _node* next;
} deque_node_t;

/**
 * @brief Estrutura de deque. 
 */
typedef struct {
    deque_node_t* head;
    deque_node_t* tail;
} deque;

/**
 * @brief Constrói um deque vazio.
 * 
 * @return um deque.
 */
deque make_deque();

/**
 * @brief Libera os recursos alocados para um deque.
 * 
 * @note Essa função não libera os recursos dos elementos do deque; isso deve
 *       ser feito pelo usuário, seguindo o princípio de que a responsabilidade
 *       por liberar um ponteiro recai sobre quem o aloca. 
 * 
 * @param q o deque.
 */
void destroy_deque(deque* q);

/**
 * @brief Verifica se uma fila está vazia.
 * 
 * @param q a fila.
 * @return true se estiver vazia, false caso contrário.
 */
int is_empty(const deque* q);

/**
 * @brief Adiciona um elemento no fim da fila.
 * 
 * @param q a fila.
 * @param elem o elemento.
 */
void push_back(deque* q, deque_elem_t elem);

/**
 * @brief Adiciona um elemento no começo da fila.
 * 
 * @param q a fila.
 * @param elem o elemento.
 */
void push_front(deque* q, deque_elem_t elem);

/**
 * @brief Retorna o primeiro elemento da fila.
 * 
 * @note Essa função assume que a fila não está vazia. Seu comportamento é
 *       indefinido caso chamada em uma fila vazia.
 * 
 * @param q a fila.
 * @return o elemento.
 */
deque_elem_t front(const deque* q);

/**
 * @brief Remove um elemento do começo da fila.
 * 
 * @note Essa função não libera a memória do elemento armazenado, isso deve ser
 *       feito pelo usuário caso necessário.
 * 
 * @param q a fila.
 */
void pop_front(deque* q);

/**
 * @brief Remove e libera um nó particular da fila.
 * 
 * @note A função assume que o nó faz parte da fila. Caso não seja, seu
 *       comportamento é indefinido.
 * 
 * @param q o deque.
 * @param node o nó.
 */
void drop_node(deque* q, deque_node_t* node);

#endif // __DEQUE_H__
