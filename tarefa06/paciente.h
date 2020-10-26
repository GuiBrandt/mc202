/**
 * @file paciente.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT para um paciente.
 */

#ifndef __PACIENTE_H__
#define __PACIENTE_H__

#include "deque.h"

// Tamanho máximo de nome de paciente
#define MAX_NOME 50

/**
 * @brief Enumeração de tipos de prioridade (normal/preferencial)
 */
typedef enum {
    NORMAL,
    PREFERENCIAL
} prioridade_t;

/**
 * @brief Estrutura de informações de paciente. 
 */
typedef struct {
    char nome[MAX_NOME + 1];
    prioridade_t prioridade;
    int sendo_atendido;
    deque atendimentos;
} paciente_t;

/**
 * @brief Lê uma entrada de paciente da entrada padrão.
 * 
 * @return um ponteiro para um paciente lido alocado dinamicamente.
 */
paciente_t* ler_paciente();

#endif // __PACIENTE_H__
