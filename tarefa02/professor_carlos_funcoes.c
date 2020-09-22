/**
 * @file professor_carlos_funcoes.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa02.html
 */

#include "professor_carlos.h"

#include <assert.h>

// Constantes de comparação
#define MENOR -1  // Comparador "<"
#define MAIOR  1  // Comparador ">"

/**
 * Compara duas datas.
 * 
 * @param lhs Lado esquerdo da comparação.
 * @param rhs Lado direito da comparação.
 * 
 * @returns -1 se lhs < rhs, 1 se lhs > rhs e 0 se forem iguais.
 */
int compara_data(Data lhs, Data rhs) __attribute_const__;
int compara_data(Data lhs, Data rhs) {
    if (lhs.ano < rhs.ano) return -1;
    else if (lhs.mes < rhs.mes) return -1;
    else if (lhs.dia < rhs.dia) return -1;
    else if (lhs.ano == rhs.ano && lhs.mes == rhs.mes && lhs.dia == rhs.dia) return 0;
    else return 1;
}

/**
 * Busca pelo aluno mais novo ou mais velho em uma turma.
 * 
 * @param t Lista de turmas.
 * @param qtd_turmas Quantidade de turmas.
 * @param j Índice da turma desejada.
 * @param minmax MENOR (-1, mais novo) ou MAIOR (1, mais velho).
 * 
 * @return Ponteiro para o aluno mais novo ou mais velho na turma (não-nulo). 
 */
const Aluno* procura_na_turma(Turma t[], int qtd_turmas, int j, int minmax)
    __attribute__((returns_nonnull));

const Aluno* procura_na_turma(Turma t[], int qtd_turmas, int j, int minmax) {
    assert(j < qtd_turmas);

    Turma turma = t[j];
    const Aluno* novo = &turma.alunos[0];

    for (int i = 1; i < turma.qtd; i++) {
        if (compara_data(turma.alunos[i].nascimento, novo->nascimento) * minmax >= 0) {
            novo = &turma.alunos[i];
        }
    }

    return novo;
}

/**
 * Busca pelo aluno mais novo ou mais velho em todas as turmas.
 * 
 * @param t Lista de turmas.
 * @param qtd_turmas Quantidade de turmas.
 * @param minmax MENOR (mais novo) ou MAIOR (mais velho).
 * 
 * @return Ponteiro para o aluno mais novo ou mais velho de todas as turmas (não-nulo). 
 */
const Aluno* procura_todas_turmas(Turma[], int, int)
    __attribute__((returns_nonnull));

const Aluno* procura_todas_turmas(Turma t[], int qtd_turmas, int minmax) {
    const Aluno* novo = procura_na_turma(t, qtd_turmas, 0, minmax);

    for (int i = 1; i < qtd_turmas; i++) {
        const Aluno* novo_na_turma = procura_na_turma(t, qtd_turmas, 1, minmax);

        if (compara_data(novo_na_turma->nascimento, novo->nascimento) * minmax >= 0) {
            novo = novo_na_turma;
        }
    }

    return novo;
}

Aluno procura_novo_na_turma(Turma t[], int qtd_turmas, int j) {
    return *procura_na_turma(t, qtd_turmas, j, MENOR);
}

Aluno procura_novo_todas_turmas(Turma t[], int qtd_turmas) {
    return *procura_todas_turmas(t, qtd_turmas, MENOR);
}

Aluno procura_velho_na_turma(Turma t[], int qtd_turmas, int j) {
    return *procura_na_turma(t, qtd_turmas, j, MAIOR);
}

Aluno procura_velho_todas_turmas(Turma t[], int qtd_turmas) {
    return *procura_todas_turmas(t, qtd_turmas, MAIOR);
}

/**
 * Verifica se um nome contém uma substring.
 * 
 * @param nome Nome do aluno.
 * @param padrao Substring sendo procurada (deve ser terminada com um
 *               caractere nulo).
 * 
 * @return 1 (tem a substring) ou 0 (não tem).
 */
int tem_substring(char nome[15], const char* padrao)
    __attribute_pure__
    __attribute__((nonnull(2)));

int tem_substring(char nome[15], const char* padrao) {

    // Busca pela primera letra do padrão no nome
    int i_nome = 0;
    while (i_nome < 15 && nome[i_nome] != padrao[0]) i_nome++;

    if (i_nome < 15) {
        // Se achar a primeira letra, continua comparando o padrão com o nome
        // a partir dali
        int i_padrao = 1;
        while (i_nome + i_padrao < 15 && padrao[i_padrao] != 0) {
            if (nome[i_nome + i_padrao] != padrao[i_padrao]) return 0;
            
            i_padrao++;
        }

        return padrao[i_padrao] == 0; // Se chegou no '\0', leu o padrão inteiro.

    } else { // Se não achar a primeira letra, certamente o padrão não existe
        return 0;
    }
}

int conta_substrings(Turma t[], int qtd_turmas, char *padrao) {
    int contador = 0;

    for (int i = 0; i < qtd_turmas; i++) {
        Turma turma = t[i];

        for (int j = 0; j < turma.qtd; j++) {
            contador += tem_substring(turma.alunos[j].nome, padrao);
        }
    }

    return contador;
}
