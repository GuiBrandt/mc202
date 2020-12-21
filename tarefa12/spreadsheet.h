/**
 * @file spreadsheet.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT de planilha.
 */

#ifndef __SPREADSHEET_H__
#define __SPREADSHEET_H__

#include <stdio.h>
#include <stddef.h>

/**
 * @brief Estrutura opaca de planilha. 
 */
typedef struct spreadsheet spreadsheet_t;

/**
 * @brief Aloca, lê e retorna uma planilha de um arquivo.
 * 
 * A memória retornada por esta função deve ser liberada pelo usuário através
 * da função destroy_spreadsheet.
 * 
 * @param file ponteiro para o handle do arquivo.
 * @param rows número de linhas na planilha.
 * @param cols número de colunas na planilha.
 * 
 * @return um ponteiro para uma planilha alocada e preenchida com os valores
 *         correspondentes ao arquivo dado.
 */
spreadsheet_t* read_spreadsheet(FILE* file, size_t rows, size_t cols);

/**
 * @brief Calcula o valor de uma célula da planilha.
 * 
 * Excepcionalmente, esta função retorna INT_MIN quando a célula dada é
 * inválida (i.e. apresenta dependências cíclicas).
 * 
 * @param spreadsheet a planilha.
 * @param column nome da coluna (A-Z).
 * @param row número da linha.
 * 
 * @return valor inteiro calculado usando o valor da célula.
 */
int spreadsheet_eval(spreadsheet_t* spreadsheet, char column, size_t row);

/**
 * @brief Atualiza o valor de uma célula da planilha.
 * 
 * Assume que a célula sendo alterada tem valor constante.
 * 
 * @param spreadsheet a planilha.
 * @param column nome da coluna (A-Z).
 * @param row número da linha.
 * @param value valor a ser atribuído.
 * 
 * @return o valor atribuído à celula. 
 */
int spreadsheet_update(
    spreadsheet_t* spreadsheet,
    char column,
    size_t row,
    int value
);

/**
 * @brief Libera os recursos alocados para uma planilha.
 * 
 * @param spreadsheet a planilha.
 */
void destroy_spreadsheet(spreadsheet_t* spreadsheet);

#endif // __SPREADSHEET_H__
