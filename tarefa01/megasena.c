/**
 * @file megasena.c
 * @author Guilherme G. Brandt
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa01.html
 */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

//=============================================================================
// DEFINIÇÕES
//=============================================================================

// Tipo booleano
#define bool int

// Tipo para a matriz booleana do volante
typedef bool** bool_matrix;
typedef const int* const* const_bool_matrix;

// Constantes do jogo
#define M_COLS 6
#define M_ROWS 10
#define N_LUCKY_NUMBERS 6

// Números sorteados
int lucky_numbers[N_LUCKY_NUMBERS];

/**
 * Aloca e retorna uma matriz no heap.
 * 
 * @return Uma matriz booleana alocada no heap.
 */
bool_matrix alloc_matrix() __attribute_malloc__;

/**
 * Libera a memória alocada para uma matriz.
 * 
 * @param matrix Matriz.
 */
void free_matrix(bool_matrix matrix);

/**
 * Lê uma matriz da entrada padrão
 * 
 * @param matrix Ponteiro para uma matriz a ser preenchida.
 */
void read_matrix(bool_matrix *matrix)
    __attribute__ ((
#if __GNUC__ >= 10
    access (write_only, 1),
#endif
    nonnull (1)));

/**
 * Calcula a quantidade de acertos para cada participante
 * 
 * @param m Número de participantes.
 * @param matrices Lista de matrizes dos participantes.
 * @param hits Lista de acertos dos participantes.
 */
void calc_hits(int m, const_bool_matrix* matrices, int* hits)
    __attribute__ ((
#if __GNUC__ >= 10
        access (read_only, 2, 1),
        access (write_only, 3, 1),
#endif
        nonnull (2, 3)
    ));

//=============================================================================
// PONTO DE ENTRADA
//=============================================================================

int main(int argc, char** argv) {

    // Lê o número de participantes e o prêmio
    int m;
    double n;

    scanf("%d %lf", &m, &n);

    // Aloca a primeira linha de ponteiros para as matrizes de
    // cada participante e lê as matrizes
    bool_matrix* matrices = (bool_matrix*) malloc(m * sizeof(bool_matrix));
    for (int i = 0; i < m; i++) {
        matrices[i] = alloc_matrix();
        read_matrix(&matrices[i]);
    }

    // Lê os números sorteados
    for (int i = 0; i < N_LUCKY_NUMBERS; i++) scanf("%d ", &lucky_numbers[i]);

    // Calcula o total de acertos por participante
    int* hits = (int*) calloc(m, sizeof(int));
    calc_hits(m, (const_bool_matrix*) matrices, hits);

    // Conta o número de ganhadores por categoria
    int winners_by_category[3] = { 0, 0, 0 };
    for (int i = 0; i < m; i++) {
        if (hits[i] < 4) continue;
        
        winners_by_category[hits[i] - 4]++;
    }

    // Finalmente, mostra os resultados
    double prizes[3] = {
        .19 * n / winners_by_category[0],
        .19 * n / winners_by_category[1],
        .62 * n / winners_by_category[2]
    };

    for (int i = 0; i < m; i++) {
        if (hits[i] < 4) printf("0.00\n");
        else printf("%.02f\n", prizes[hits[i] - 4]);
    }

    // Limpeza de memória
    free(hits);

    for (int i = 0; i < m; i++) free_matrix(matrices[i]);
    free(matrices);

    return 0;
}

//=============================================================================
// IMPLEMENTAÇÕES
//=============================================================================

bool_matrix alloc_matrix() {
    bool_matrix allocated = (bool_matrix) malloc(M_ROWS * sizeof(bool*));

    for (int i = 0; i < M_ROWS; i++) {
        allocated[i] = (bool*) malloc(M_COLS * sizeof(bool));
    }

    return allocated;
}

void free_matrix(bool_matrix matrix) {
    for (int i = 0; i < M_ROWS; i++) free(matrix[i]);
    free(matrix);
}

void read_matrix(bool_matrix* matrix) {
    assert(matrix != NULL);

    for (int i = 0; i < M_ROWS; i++) {
        for (int j = 0; j < M_COLS; j++) scanf("%d ", &(*matrix)[i][j]);
    }
}

void calc_hits(int m, const_bool_matrix *matrices, int *hits) {
    assert(matrices != NULL);
    assert(hits != NULL);

    for (int i = 0; i < m; i++) {
        const_bool_matrix matrix = matrices[i];

        for (int l = 0; l < N_LUCKY_NUMBERS; l++) {
            int n = lucky_numbers[l] - 1;
            if (matrix[n / M_COLS][n % M_COLS]) hits[i]++;
        }
    }
}
