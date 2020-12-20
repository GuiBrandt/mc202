#include "spreadsheet.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>

#include "xmalloc.h"
#include "expressions.h"

typedef struct cell {
    expression_t* expression;
} cell;

typedef cell row[26];

struct spreadsheet {
    size_t n_rows;
    size_t rows_capacity;
    row* rows;
};

#define INITIAL_CAPACITY 64

spreadsheet_t* make_spreadsheet() {
    spreadsheet_t* s = (spreadsheet_t*) xmalloc(sizeof(spreadsheet_t));
    s->n_rows = 0;
    s->rows_capacity = INITIAL_CAPACITY;

    size_t rows_size = sizeof(row) * s->rows_capacity;
    s->rows = (row*) xmalloc(rows_size);
    memset(s->rows, 0, rows_size);

    return s;
}

void grow(spreadsheet_t* spreadsheet) {
    spreadsheet->rows_capacity *= 2;

    size_t rows_size = sizeof(row) * spreadsheet->rows_capacity;
    spreadsheet->rows = (row*) realloc(spreadsheet->rows, rows_size);
    
    if (spreadsheet->rows == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory.");
        exit(-1);
    }

    memset(&spreadsheet->rows[spreadsheet->n_rows], 0, rows_size / 2);
}

row* next_row(spreadsheet_t* spreadsheet) {
    if (spreadsheet->n_rows == spreadsheet->rows_capacity) {
        grow(spreadsheet);
    }

    return &spreadsheet->rows[spreadsheet->n_rows++];
}

bool read_row(FILE* file, spreadsheet_t* spreadsheet) {
    char* cell_str;
    int read = fscanf(file, " %m[^,] ", &cell_str);

    if (read != 1) {
        return false;
    }

    row* r = next_row(spreadsheet);

    int i = 0;
    do {
        assert(i < 26);
        (*r)[i++].expression = parse(cell_str);
        free(cell_str);

        read = fscanf(file, " , %m[^,\n] ", &cell_str);
    } while (read == 1);

    printf("\n");

    return true;
}

expression_t* spreadsheet_get(spreadsheet_t* s, char column, size_t row) {
    assert('A' <= column && column <= 'Z');
    assert(0 < row && row <= s->n_rows);

    // TODO: ciclos

    expression_t* expr = s->rows[row - 1][column - 'A'].expression;

    return expr;
}

spreadsheet_t* read_spreadsheet(FILE* file) {
    spreadsheet_t* spreadsheet = make_spreadsheet();
    
    while (read_row(file, spreadsheet))
        ;

    return spreadsheet;
}

int spreadsheet_eval(spreadsheet_t* spreadsheet, char column, size_t row) {
    expression_t* expr = spreadsheet_get(spreadsheet, column, row);
    return eval(expr, (void*) spreadsheet, (resolve_fn_t) spreadsheet_get);
}

int spreadsheet_update(spreadsheet_t* spreadsheet, char column, size_t row, int value);

void destroy_spreadsheet(spreadsheet_t* spreadsheet);

int main() {
    FILE* csv = fopen("testes/planilha1.csv", "r");
    spreadsheet_t* s = read_spreadsheet(csv);
    fclose(csv);

    printf("%d\n", spreadsheet_eval(s, 'B', 2));

    return 0;
}
