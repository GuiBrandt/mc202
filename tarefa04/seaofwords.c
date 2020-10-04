/**
 * @file seaofwords.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa04.html 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//=============================================================================
// DEFINIÇÕES
//=============================================================================

#define MAX_LINHAS 100
#define MAX_COLUNAS 100

#define MAX_LEN_PALAVRA 20
#define MAX_PALAVRAS 10

/**
 * Verifica se uma palavra existe no texto.
 * 
 * @param n - número de linhas no texto.
 * @param m - número de colunas no texto.
 * @param texto - matriz de texto.
 * @param palavra - palavra procurada. 
 */
bool contem_palavra(
    int n, int m,
    const char texto[][MAX_COLUNAS],
    const char* palavra
);

//=============================================================================
// PONTO DE ENTRADA
//=============================================================================

int main() {
    int n, m, q;
    scanf("%d %d %d", &n, &m, &q);

    char texto[MAX_LINHAS][MAX_COLUNAS];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf(" %c", &texto[i][j]);
        }
    }

    for (int i = 0; i < q; i++) {
        char palavra[MAX_LEN_PALAVRA + 1];
        scanf("%s", palavra);

        if (contem_palavra(n, m, texto, palavra)) printf("sim\n");
        else printf("nao\n");
    }

    return 0;
}

//=============================================================================
// IMPLEMENTAÇÕES
//=============================================================================

bool contem_palavra_rec(
    int n, int m,
    const char texto[][MAX_COLUNAS],
    bool usado[][MAX_COLUNAS],
    int l, int c,
    const char* palavra
) __attribute__((hot));

bool contem_palavra_rec(
    int n, int m,
    const char texto[][MAX_COLUNAS],
    bool usado[][MAX_COLUNAS],
    int l, int c,
    const char* palavra
) {
    // Caso base 1: palavra vazia, i.e. encontrou a palavra completa
    if (palavra[0] == 0)
        return true;

    // Caso base 2: fora da matriz
    else if (l < 0 || c < 0 || l >= n || c >= m)
        return false;
    
    // Caso base 3: posição já usada ou incorreta
    else if (usado[l][c] || texto[l][c] != palavra[0])
        return false;

    // Passo recursivo: a primeira letra da palavra bate!
    else {
        // Marca o caractere lido como usado, para não repetir
        usado[l][c] = true;

        // Visita os vizinhos para procurar o resto da palavra
        for (int il = -1; il <= 1; il++) {
            for (int ic = -1; ic <= 1; ic++) {
                if (ic != 0 && il != 0) continue; // Ignora diagonais

                // Busca recursiva na célula vizinha
                if (contem_palavra_rec(n, m, texto, usado, l + il, c + ic, palavra + 1))
                    return true;
            }
        }

        // Se não achou a próxima letra em nenhum vizinho, a palavra não
        // começa aqui.
        usado[l][c] = false; // "Desusa" a posição: passo de retrocesso.
        return false;
    }
}

bool contem_palavra(
    int n, int m,
    const char texto[][MAX_COLUNAS],
    const char* palavra
) {
    bool usado[MAX_LINHAS][MAX_COLUNAS] = { false };

    // Faz a busca recursiva com backtracking para cada posição na matriz
    for (int l = 0; l < n; l++) {
        for (int c = 0; c < m; c++) {
            if (contem_palavra_rec(n, m, texto, usado, l, c, palavra)) return true;
        }
    }

    return false;
}
