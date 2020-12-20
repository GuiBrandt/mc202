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

struct spreadsheet {
    size_t n_rows;
    size_t n_cols;
    cell** cells;
};

#define INITIAL_CAPACITY 64

spreadsheet_t* make_spreadsheet(size_t rows, size_t cols) {
    spreadsheet_t* s = (spreadsheet_t*) xmalloc(sizeof(spreadsheet_t));
    s->n_rows = rows;
    s->n_cols = cols;

    s->cells = (cell**) xmalloc(sizeof(cell*) * rows);
    for (int i = 0; i < rows; i++) {
        s->cells[i] = (cell*) xmalloc(sizeof(cell) * cols);
        memset(s->cells[i], 0, sizeof(cell) * cols);
    }

    return s;
}

void read_row(FILE* file, cell* row, size_t cols) {
    char* cell_str;

    int read = fscanf(file, " %m[^,] ", &cell_str);
    assert(read == 1);

    int i = 0;
    do {
        assert(read == 1);

        row[i++].expression = parse(cell_str);
        free(cell_str);

        read = fscanf(file, " , %m[^,\n] ", &cell_str);
    } while (i < cols);
}

cell* spreadsheet_cell(spreadsheet_t* s, char column, size_t row) {
    assert('A' <= column && column <= 'Z');
    assert(0 < row && row <= s->n_rows);

    return &s->cells[row - 1][column - 'A'];
}

expression_t* spreadsheet_get(spreadsheet_t* s, char column, size_t row) {
    cell* c = spreadsheet_cell(s, column, row);
    // TODO: ciclos
    return c->expression;
}

spreadsheet_t* read_spreadsheet(FILE* file, size_t rows, size_t cols) {
    spreadsheet_t* spreadsheet = make_spreadsheet(rows, cols);
    
    for (int i = 0; i < rows; i++) {
        read_row(file, spreadsheet->cells[i], cols);
    }

    return spreadsheet;
}

int spreadsheet_eval(spreadsheet_t* spreadsheet, char column, size_t row) {
    expression_t* expr = spreadsheet_get(spreadsheet, column, row);
    return eval(expr, (void*) spreadsheet, (resolve_fn_t) spreadsheet_get);
}

int spreadsheet_update(spreadsheet_t* spreadsheet, char column, size_t row, int value) {
    cell* c = spreadsheet_cell(spreadsheet, column, row);
    destroy_expression(c->expression);
    c->expression = pure(value);
    return value;
}

void destroy_spreadsheet(spreadsheet_t* spreadsheet) {
    for (int i = 0; i < spreadsheet->n_rows; i++) {
        for (int j = 0; j < spreadsheet->n_cols; j++) {
            destroy_expression(spreadsheet->cells[i][j].expression);
        }

        free(spreadsheet->cells[i]);
    }

    free(spreadsheet->cells);
}