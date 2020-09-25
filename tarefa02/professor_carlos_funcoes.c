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
 * @returns int < 0 se lhs < rhs, int > 0 se lhs > rhs e 0 se forem iguais.
 */
int compara_data(Data lhs, Data rhs) __attribute_const__;

int compara_data(Data lhs, Data rhs) {
    if (lhs.ano != rhs.ano) return lhs.ano - rhs.ano;
    else if (lhs.mes != rhs.mes) return lhs.mes - rhs.mes;
    else if (lhs.dia != rhs.dia) return lhs.dia - rhs.dia;
    return 0;
}

/**
 * Compara dois nomes/sobrenomes.
 * 
 * @param lhs Lado esquerdo da comparação.
 * @param rhs Lado direito da comparação.
 * 
 * @returns -1 se lhs < rhs, 1 se lhs > rhs e 0 se forem iguais.
 */
int compara_string(const char lhs[15], const char rhs[15]) __attribute_pure__;

int compara_string(const char lhs[15], const char rhs[15]) {
    for (int i = 0; i < 15; i++) {
        if (lhs[i] < rhs[i]) return -1;
        else if (lhs[i] > rhs[i]) return 1;
        else if (!lhs[i]) return 0; // Se os caracteres são iguais e ambos 0,
                                    // chegou no fim das strings.
    }

    return 0;
}

/**
 * Compara dois alunos por nascimento, nome e sobrenome (em ordem
 * lexicográfica).
 * 
 * @param lhs Lado esquerdo da comparação.
 * @param rhs Lado direito da comparação.
 * @param minmax MENOR (-1) => mais velho = menor, MAIOR (1) => mais velho = maior.
 * 
 * @returns -1 se lhs < rhs, 1 se lhs > rhs e 0 se forem iguais.
 */
int compara_aluno(Aluno lhs, Aluno rhs, int minmax) __attribute_const__;

int compara_aluno(Aluno lhs, Aluno rhs, int minmax) {
    int cmp = minmax * compara_data(lhs.nascimento, rhs.nascimento);

    // Se a data for igual, compara nome e depois sobrenome
    if (cmp == 0) cmp = compara_string(lhs.nome, rhs.nome);
    if (cmp == 0) cmp = compara_string(lhs.sobrenome, rhs.sobrenome);

    return cmp;
}

/**
 * Busca pelo aluno mais novo ou mais velho em uma turma.
 * 
 * @param t Lista de turmas.
 * @param qtd_turmas Quantidade de turmas.
 * @param j Índice da turma desejada.
 * @param minmax MENOR (-1, mais novo) ou MAIOR (1, mais velho).
 * 
 * @return Aluno mais novo ou mais velho na turma. 
 */
Aluno procura_na_turma(const Turma t[], int qtd_turmas, int j, int minmax) {
    assert(j < qtd_turmas);

    Turma turma = t[j];
    Aluno min = turma.alunos[0];

    for (int i = 1; i < turma.qtd; i++) {   
        Aluno a = turma.alunos[i];

        if (compara_aluno(a, min, minmax) < 0) {
            min = a;
        }
    }

    return min;
}

/**
 * Busca pelo aluno mais novo ou mais velho em todas as turmas.
 * 
 * @param t Lista de turmas.
 * @param qtd_turmas Quantidade de turmas.
 * @param minmax MENOR (mais novo) ou MAIOR (mais velho).
 * 
 * @return Aluno mais novo ou mais velho de todas as turmas. 
 */
Aluno procura_todas_turmas(const Turma t[], int qtd_turmas, int minmax) {
    Aluno min = procura_na_turma(t, qtd_turmas, 0, minmax);

    for (int i = 1; i < qtd_turmas; i++) {
        Aluno novo_na_turma = procura_na_turma(t, qtd_turmas, i, minmax);

        if (compara_aluno(novo_na_turma, min, minmax) < 0) {
            min = novo_na_turma;
        }
    }

    return min;
}

Aluno procura_novo_na_turma(Turma t[], int qtd_turmas, int j) {
    return procura_na_turma(t, qtd_turmas, j, MENOR);
}

Aluno procura_novo_todas_turmas(Turma t[], int qtd_turmas) {
    return procura_todas_turmas(t, qtd_turmas, MENOR);
}

Aluno procura_velho_na_turma(Turma t[], int qtd_turmas, int j) {
    return procura_na_turma(t, qtd_turmas, j, MAIOR);
}

Aluno procura_velho_todas_turmas(Turma t[], int qtd_turmas) {
    return procura_todas_turmas(t, qtd_turmas, MAIOR);
}

/**
 * Busca uma letra em um nome, a partir de um índice.
 * 
 * @param nome Nome.
 * @param inicio Índice inicial da busca.
 * @param letra Letra a ser buscada.
 * 
 * @return o índice da letra no nome, ou -1 caso ela não exista.
 */
int busca_letra(const char nome[15], int inicio, char letra)
    __attribute_pure__
#if __GNUC__ >= 10
    __attribute__((read_only(1)))
#endif
    __attribute__((hot, optimize(3)));

int busca_letra(const char* restrict nome, int inicio, char letra) {
    for (int i = inicio; nome[i] != 0; i++) {
        if (nome[i] == letra) return i;
    }

    return -1;
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
int tem_substring(const char nome[15], const char* padrao)
    __attribute_pure__
#if __GNUC__ >= 10
    __attribute__((read_only(1)))
#endif
    __attribute__((nonnull(2)));

int tem_substring(const char* restrict nome, const char* restrict padrao) {
    int i_nome = 0;

    do {
        i_nome = busca_letra(nome, i_nome, padrao[0]);
        if (i_nome == -1) return 0;

        // Se achar a primeira letra, continua comparando o padrão com o nome
        // a partir dali
        int i_padrao = 1;
        while (nome[i_nome + i_padrao] & padrao[i_padrao]) {
            assert(i_padrao < 5);
            
            // Se tiver diferença, para de olhar
            if (nome[i_nome + i_padrao] != padrao[i_padrao]) break;

            i_padrao++;
        }

        // Se chegou no '\0', leu o padrão inteiro.
        if (padrao[i_padrao] == 0) return 1;

        // Se deu qualquer diferença, dá pra pular até onde foi comparado.
        // Como queremos apenas detectar a existência e não contar as
        // ocorrências, não tem problema com padrões sobrepostos.
        i_nome += i_padrao;
    } while (nome[i_nome]);

    return 0;
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

int add_aluno(Turma t[], Aluno a, int j) {
    assert(t[j].qtd < 50);

    t[j].alunos[t[j].qtd] = a;
    t[j].qtd++;

    return t[j].qtd;
}

int remove_aluno(Turma t[], int j) {
    assert(t[j].qtd > 0);

    t[j].qtd--;
    return t[j].qtd;
}
