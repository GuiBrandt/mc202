/**
 * @file paciente.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de paciente.
 */

#include "paciente.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//=============================================================================
// Funções Auxiliares
//=============================================================================

/**
 * @brief Verifica se o valor de retorno de uma chamada a scanf indica falha
 *        na leitura de um padrão.
 * 
 * @param lido valor retornado pelo scanf.
 * 
 * @return 1 caso o resultado seja falha, 0 caso contrário
 */
int falhou(int lido) {
    return lido == 0 || lido == EOF;
}

/**
 * @brief Lê a informação de prioridade de atendimento para um paciente da
 *        entrada padrão.
 * 
 * @param paciente o paciente.
 */
void ler_prioridade(paciente_t* paciente) {
    char prioridade[13] = { 0 };
    scanf("%s", prioridade);

    if (strcmp(prioridade, "preferencial") == 0) {
        paciente->prioridade = PREFERENCIAL;
    } else {
        paciente->prioridade = NORMAL;
    }
}

/**
 * @brief Lê a lista de atendimentos para um paciente da entrada padrão.
 * 
 * @param paciente o paciente.
 */
void ler_atendimentos(paciente_t* paciente) {
    paciente->atendimentos = make_deque();

    int lido;
    do {
        int* especialidade = (int*) malloc(sizeof(int));
        if (especialidade == NULL) {
            fprintf(stderr, "Out of memory");
            exit(-1);
        }

        lido = scanf(" %d", especialidade);
        (*especialidade)--; // O índice começa em 0

        if (!falhou(lido)) {
            push_back(&paciente->atendimentos, especialidade);
        } else {
            free(especialidade);
        }
    } while (!falhou(lido));
}

//=============================================================================
// Contrato
//=============================================================================

paciente_t* ler_paciente() {
    char nome[MAX_NOME + 1];

    int lido = scanf("\"%[^\"]\" ", nome);
    if (falhou(lido))
        return NULL;

    paciente_t* paciente = (paciente_t*) malloc(sizeof(paciente_t));
    if (paciente == NULL) {
        fprintf(stderr, "Out of memory");
        exit(-1);
    }

    strcpy(paciente->nome, nome);
    ler_prioridade(paciente);
    ler_atendimentos(paciente);
    paciente->sendo_atendido = 0;

    return paciente;
}