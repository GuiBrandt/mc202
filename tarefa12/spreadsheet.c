#include "spreadsheet.h"

#include "expressions.h"

typedef enum cell_type {
    VALUE,
    EXPRESSION
} cell_type;

typedef struct cell {
    expression_t* expression;
} cell;

typedef cell row[26];

struct spreadsheet {
    size_t n_rows;
    size_t rows_capacity;
    row* rows;
};

spreadsheet_t* read_spreadsheet(FILE* file);
int spreadsheet_eval(char column, int row);
int spreadsheet_update(char column, int row, int value);
void destroy_spreadsheet(spreadsheet_t* spreadsheet);