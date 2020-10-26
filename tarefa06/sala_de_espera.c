/**
 * @file sala_de_espera.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa06.html
 */

#include "deque.h"

#include "paciente.h"

#include <stdlib.h>
#include <stdio.h>

//=============================================================================
// DEFINIÇÕES
//=============================================================================

/**
 * @brief Lê entrada formatada como especificado no enunciado da tarefa para
 *        uma lista de pacientes.
 * 
 * @param pacientes a lista de pacientes.
 */
void ler_entrada(deque* pacientes);

/**
 * @brief Escreve o horário de saída e o nome de um paciente.
 * 
 * @param tempo tempo de saída, cada unidade equivale a 10 minutos após as 8h
 * @param paciente o paciente.
 */
void escrever_saida(int tempo, const paciente_t* paciente);

/**
 * @brief Enfileira os pacientes nas filas de atendimento conforme necessário.
 * 
 * Esta função avança a fila de atendimentos para cada paciente enfileirado e
 * libera a memória alocada para armazenar o atendimento para o qual o paciente
 * foi enfileirado.
 * 
 * @param pacientes a lista de pacientes.
 * @param filas um array com filas para cada especialidade.
 */
void enfileirar_pacientes(deque* pacientes, deque filas[]);

/**
 * @brief Aplica o atendimento, avançando as filas e dando saída nos pacientes
 *        que não têm mais atendimentos marcados.
 * 
 * Esta função libera a memória alocada para pacientes cujo atendimento
 * terminou.
 * 
 * @param tempo tempo de atendimento, cada unidade equivale a 10 minutos após
 *              as 8h
 * @param pacientes a lista de pacientes. 
 * @param filas um array com filas para cada especialidade.
 */
void atender_pacientes(int tempo, deque* pacientes, deque filas[]);

//=============================================================================
// PONTO DE ENTRADA
//=============================================================================

int main() {
    deque filas[9];
    for (int i = 0; i < 9; i++) {
        filas[i] = make_deque();
    }

    deque pacientes = make_deque();
    ler_entrada(&pacientes);

    for (int tempo = 1; !is_empty(&pacientes); tempo++) {
        enfileirar_pacientes(&pacientes, filas);
        atender_pacientes(tempo, &pacientes, filas);
    }

    destroy_deque(&pacientes);

    for (int i = 0; i < 9; i++) {
        destroy_deque(&filas[i]);
    }

    return 0;
}

//=============================================================================
// IMPLEMENTAÇÕES (Atendimento)
//=============================================================================

void enfileirar_pacientes(deque* pacientes, deque filas[]) {
    for (deque_node_t* it = pacientes->head; it != NULL; it = it->next) {
        paciente_t* paciente = (paciente_t*) it->elem;

        if (paciente->sendo_atendido)
            continue;

        paciente->sendo_atendido = 1;
        
        deque* atendimentos = &paciente->atendimentos;

        int* especialidade = (int*) front(atendimentos);
        deque* fila = &filas[*especialidade];
        free(especialidade);

        if (paciente->prioridade == PREFERENCIAL) {
            push_front(fila, it);
        } else {
            push_back(fila, it);
        }
    }
}

static int CAPACIDADE[] = {
    10,
    2,
    5,
    3,
    4,
    7,
    2,
    1,
    4
};

void atender_pacientes(int tempo, deque* pacientes, deque filas[]) {
    for (int i = 0; i < 9; i++) {
        deque* fila = &filas[i];

        for (int j = 0; !is_empty(fila) && j < CAPACIDADE[i]; j++) {
            deque_node_t* node = (deque_node_t*) front(fila);
            pop_front(fila);

            paciente_t* paciente = (paciente_t*) node->elem;
            paciente->sendo_atendido = 0;

            pop_front(&paciente->atendimentos);
            
            if (is_empty(&paciente->atendimentos)) {
                drop_node(pacientes, node);
                escrever_saida(tempo, paciente);
                free(paciente);
            }
        }
    }
}

//=============================================================================
// IMPLEMENTAÇÕES (I/O)
//=============================================================================

void ler_entrada(deque* pacientes) {
    paciente_t* paciente;
    do {
        paciente = ler_paciente();

        if (paciente) {
            push_back(pacientes, paciente);
        }
    } while (paciente);
}

void escrever_saida(int tempo, const paciente_t* paciente) {
    int hora = 8 + tempo / 6;
    int minuto = (tempo % 6) * 10;
    printf("%02d:%02d %s\n", hora, minuto, paciente->nome);
}
