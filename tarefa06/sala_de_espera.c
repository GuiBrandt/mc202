/**
 * @file sala_de_espera.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa06.html
 */

#include "deque.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define MAX_NOME 50

typedef enum {
    NORMAL,
    PREFERENCIAL
} prioridade_t;

typedef struct {
    char nome[MAX_NOME + 1];
    prioridade_t prioridade;
    deque atendimentos;
    int sendo_atendido;
} paciente;

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

paciente* ler_paciente();

int main() {
    deque filas[9];
    for (int i = 0; i < 9; i++)
        filas[i] = make_deque();

    deque pacientes = make_deque();

    paciente* p;
    do {
        p = ler_paciente();

        if (p) {
            push_back(&pacientes, p);
        }
    } while (p);

    for (int tempo = 1; !is_empty(&pacientes); tempo++) {
        for (deque_node_t* it = pacientes.head; it != NULL; it = it->next) {
            paciente* p = (paciente*) it->elem;

            if (p->sendo_atendido)
                continue;

            deque* atendimentos = &p->atendimentos;
            
            int* especialidade = front(atendimentos);
            p->sendo_atendido = 1;

            deque* fila = &filas[*especialidade];

            free(especialidade);

            if (p->prioridade == PREFERENCIAL) {
                push_front(fila, it);
            } else {
                push_back(fila, it);
            }
        }

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < CAPACIDADE[i]; j++) {
                if (is_empty(&filas[i]))
                    continue;

                deque_node_t* node = front(&filas[i]);
                paciente* p = node->elem;

                pop_front(&filas[i]);
                p->sendo_atendido = 0;

                pop_front(&p->atendimentos);
                if (is_empty(&p->atendimentos)) {
                    if (pacientes.head == node) {
                        pacientes.head = node->next;
                    } else {
                        node->prev->next = node->next;
                    }

                    if (pacientes.tail == node) {
                        pacientes.tail = node->prev;
                    } else {
                        node->next->prev = node->prev;
                    }

                    free(node);
                }

                if (is_empty(&p->atendimentos)) {
                    printf("%02d:%02d %s\n", 8 + tempo / 6, (tempo % 6) * 10, p->nome);
                    free(p);
                }
            }
        }
    }

    destroy_deque(&pacientes);

    for (int i = 0; i < 9; i++)
        destroy_deque(&filas[i]);

    return 0;
}

paciente* ler_paciente() {
    char nome[MAX_NOME + 1];
    int lido = scanf("\"%[^\"]\" ", nome);
    if (lido == 0 || lido == EOF)
        return NULL;

    paciente* p = (paciente*) malloc(sizeof(paciente));
    if (p == NULL) {
        fprintf(stderr, "Out of memory");
        exit(-1);
    }

    strcpy(p->nome, nome);

    char prioridade[13] = { 0 };
    scanf("%s", prioridade);

    if (strcmp(prioridade, "preferencial") == 0) {
        p->prioridade = PREFERENCIAL;
    } else {
        p->prioridade = NORMAL;
    }

    p->atendimentos = make_deque();

    do {
        int* especialidade = (int*) malloc(sizeof(int));
        if (especialidade == NULL) {
            fprintf(stderr, "Out of memory");
            exit(-1);
        }

        lido = scanf(" %d", especialidade);
        (*especialidade)--;

        if (lido != 0 && lido != EOF) {
            push_back(&p->atendimentos, especialidade);
        } else {
            free(especialidade);
        }
    } while (lido != 0 && lido != EOF);

    p->sendo_atendido = 0;

    return p;
}
