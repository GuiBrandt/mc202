/**
 * @file planilha.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação da lógica da tarefa.
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa12.html 
 */

#include <stddef.h>
#include <stdio.h>

#include <limits.h>
#include <assert.h>

#include "spreadsheet.h"

void get(spreadsheet_t* spreadsheet, char col, size_t row);
void set(spreadsheet_t* spreadsheet, char col, size_t row, int value);

int main() {
    char filename[128];
    size_t columns, rows;
    scanf("%s %lu %lu ", filename, &columns, &rows);

    FILE* csv = fopen(filename, "r");
    spreadsheet_t* spreadsheet = read_spreadsheet(csv, rows, columns);
    fclose(csv);

    char op;
    char col;
    size_t row;

    int read;
    while ((read = scanf("%c %c%lu ", &op, &col, &row)) > 0) {
        if (op == 'G') {
            get(spreadsheet, col, row);

        } else {
            assert(op == 'S');

            int value;
            scanf("%d ", &value);
            set(spreadsheet, col, row, value);
        }
    }

    destroy_spreadsheet(spreadsheet);

    return 0;
}

void get(spreadsheet_t* spreadsheet, char col, size_t row) {
    printf("%c%lu: ", col, row);

    int value = spreadsheet_eval(spreadsheet, col, row);
    
    if (value != INT_MIN) {
        printf("%d", value);
    } else {
        printf("#ERRO#");
    }

    printf("\n");
}

void set(spreadsheet_t* spreadsheet, char col, size_t row, int value) {

    int old = spreadsheet_eval(spreadsheet, col, row);

    spreadsheet_update(spreadsheet, col, row, value);

    printf("%c%lu: %d -> %d\n", col, row, old, value);
}
