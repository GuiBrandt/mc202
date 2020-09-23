/**
 * @file professor_carlos_main.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa02.html
 */

#include <stdio.h>
#include <assert.h>

#include "professor_carlos.h"

//=============================================================================
// DEFINIÇÕES
//=============================================================================

/**
 * Lê as informações de uma turma da entrada padrão.
 * 
 * @param t Ponteiro para a turma.
 */
void ler_turma(Turma* t)
    __attribute__ ((
#if __GNUC__ >= 10
    access (write_only, 1),
#endif
    nonnull (1)));

/**
 * Lê as informações de um aluno da entrada padrão.
 * 
 * @param a Ponteiro para o aluno.
 */
void ler_aluno(Aluno* a)
    __attribute__ ((
#if __GNUC__ >= 10
    access (write_only, 1),
#endif
    nonnull (1)));

/**
 * Tipo de função de execução de comando.
 * 
 * @param t Lista de turmas.
 * @param qtd_turmas Quantidade de turmas.
 */
typedef void (*comando_main_t)(Turma t[], int qtd_turmas);

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

/** Comando para obter o nome do aluno mais novo de uma turma */
void nome_mais_novo_turma_main(Turma t[], int qtd_turmas);

/** Comando para obter o sobrenome do aluno mais velho de uma turma */
void sobrenome_mais_velho_turma_main(Turma t[], int qtd_turmas);

/** Comando para obter o nome do aluno mais velho de todas as turmas */
void nome_mais_velho_todos_main(Turma t[], int qtd_turmas);

/** Comando para obter o sobrenome do aluno mais novo de todas as turmas */
void sobrenome_mais_novo_todos_main(Turma t[], int qtd_turmas);

/** Comando para contar o número de ocorrências de uma substring nos nomes */
void contar_substrings_main(Turma t[], int qtd_turmas);

/** Comando para adicionar aluno */
void adiciona_aluno_main(Turma t[], int qtd_turmas);

/** Comando para remover aluno */
void remove_aluno_main(Turma t[], int qtd_turmas);

/**
 * Mapeamento de códigos de comando para funções de comando
 */
comando_main_t comandos[] = {
    [NOME_MAIS_NOVO_TURMA] = nome_mais_novo_turma_main,
    [SOBRENOME_MAIS_VELHO_TURMA] = sobrenome_mais_velho_turma_main,
    [NOME_MAIS_VELHO_TODOS] = nome_mais_velho_todos_main,
    [SOBRENOME_MAIS_NOVO_TODOS] = sobrenome_mais_novo_todos_main,
    [CONTAR_SUBSTRINGS] = contar_substrings_main,
    [ADICIONA_ALUNO] = adiciona_aluno_main,
    [REMOVE_ALUNO] = remove_aluno_main
};

//=============================================================================
// PONTO DE ENTRADA
//=============================================================================

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

        comandos[comando](t, n);
    }
}

//=============================================================================
// IMPLEMENTAÇÕES (Comandos)
//=============================================================================

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

void contar_substrings_main(Turma t[], int qtd_turmas) {
    char substring[6] = { 0 };
    scanf("%s", substring);

    int contagem = conta_substrings(t, qtd_turmas, substring);
    printf("%d\n", contagem);
}

void adiciona_aluno_main(Turma t[], int qtd_turmas) {
    int j;
    scanf("%d", &j);

    Aluno a;
    ler_aluno(&a);

    printf("%d\n", add_aluno(t, a, j));
}

void remove_aluno_main(Turma t[], int qtd_turmas) {
    int j;
    scanf("%d", &j);

    printf("%d\n", remove_aluno(t, j));
}

//=============================================================================
// IMPLEMENTAÇÕES (Auxiliares)
//=============================================================================

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
