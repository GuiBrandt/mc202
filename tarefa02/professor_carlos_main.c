/**
 * @file professor_carlos_main.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa02.html
 */

#include <stdio.h>
#include <assert.h>

#include "professor_carlos.h"

/**
 * Enumeração de comandos.
 */
typedef enum _comando {
    NOME_MAIS_NOVO_TURMA = 1,
    SOBRENOME_MAIS_VELHO_TURMA,
    NOME_MAIS_VELHO_TODOS,
    SOBRENOME_MAIS_NOVO_TODOS,
    CONTAR_SUBSTRINGS,
    ADICIONA_ALUNO,
    REMOVE_ALUNO
} Comando;

/**
 * Lê as informações de uma turma da entrada padrão.
 * 
 * @param t Ponteiro para a turma.
 */
void ler_turma(Turma* t);

/**
 * Lê as informações de um aluno da entrada padrão.
 * 
 * @param a Ponteiro para o aluno.
 */
void ler_aluno(Aluno* a);

void nome_mais_novo_turma_main(Turma t[], int qtd_turmas);
void sobrenome_mais_velho_turma_main(Turma t[], int qtd_turmas);
void nome_mais_velho_todos_main(Turma t[], int qtd_turmas);
void sobrenome_mais_novo_todos_main(Turma t[], int qtd_turmas);
void contar_substrings_main(Turma t[], int qtd_turmas);
void adiciona_aluno_main(Turma t[], int qtd_turmas);
void remove_aluno_main(Turma t[], int qtd_turmas);

int main() {
    int n, k;
    scanf("%d %d", &n, &k);

    Turma t[50];

    for (int i = 0; i < n; i++) {
        ler_turma(&t[i]);
    }

    for (int i = 0; i < k; i++) {
        Comando comando;
        scanf("%d", (int*) &comando);

        switch (comando) {
            case NOME_MAIS_NOVO_TURMA:
                nome_mais_novo_turma_main(t, n);
                break;

            case SOBRENOME_MAIS_VELHO_TURMA:
                sobrenome_mais_velho_turma_main(t, n);
                break;

            case NOME_MAIS_VELHO_TODOS:
                nome_mais_velho_todos_main(t, n);
                break;

            case SOBRENOME_MAIS_NOVO_TODOS:
                sobrenome_mais_novo_todos_main(t, n);
                break;

            case CONTAR_SUBSTRINGS:
                contar_substrings_main(t, n);
                break;

            case ADICIONA_ALUNO:
                adiciona_aluno_main(t, n);
                break;

            case REMOVE_ALUNO:
                remove_aluno_main(t, n);
                break;
        }
    }
}

void ler_turma(Turma* t) {
    scanf("%d", &t->qtd);

    for (int i = 0; i < t->qtd; i++) {
        ler_aluno(&t->alunos[i]);
    }
}

void ler_aluno(Aluno* a) {
    scanf("%s %s %d %d %d",
        a->nome,
        a->sobrenome,
        &a->nascimento.dia,
        &a->nascimento.mes,
        &a->nascimento.ano);
}

void nome_mais_novo_turma_main(Turma t[], int qtd_turmas) {
    int j;
    scanf("%d", &j);

    char* nome = procura_novo_na_turma(t, qtd_turmas, j).nome;
    printf("%s\n", nome);
}

void sobrenome_mais_velho_turma_main(Turma t[], int qtd_turmas) {
    int j;
    scanf("%d", &j);

    char* sobrenome = procura_velho_na_turma(t, qtd_turmas, j).sobrenome;
    printf("%s\n", sobrenome);
}

void nome_mais_velho_todos_main(Turma t[], int qtd_turmas) {
    char* nome = procura_velho_todas_turmas(t, qtd_turmas).nome;
    printf("%s\n", nome);
}

void sobrenome_mais_novo_todos_main(Turma t[], int qtd_turmas) {
    char* sobrenome = procura_novo_todas_turmas(t, qtd_turmas).sobrenome;
    printf("%s\n", sobrenome);
}

void adiciona_aluno_main(Turma t[], int qtd_turmas) {
    int j;
    scanf("%d", &j);

    Aluno a;
    ler_aluno(&a);

    printf("%d\n", add_aluno(t, a, j));
}

void contar_substrings_main(Turma t[], int qtd_turmas) {
    char substring[6] = { 0 };
    scanf("%s", substring);

    int contagem = conta_substrings(t, qtd_turmas, substring);
    printf("%d\n", contagem);
}

void remove_aluno_main(Turma t[], int qtd_turmas) {
    int j;
    scanf("%d", &j);

    printf("%d\n", remove_aluno(t, j));
}
