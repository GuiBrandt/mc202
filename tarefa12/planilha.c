#include <stddef.h>
#include <stdio.h>

#include <assert.h>

#include "spreadsheet.h"

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
            int value = spreadsheet_eval(spreadsheet, col, row);
            printf("%c%lu: %d\n", col, row, value);
        } else {
            assert(op == 'S');

            int value;
            scanf("%d ", &value);

            int old = spreadsheet_eval(spreadsheet, col, row);

            spreadsheet_update(spreadsheet, col, row, value);

            printf("%c%lu: %d -> %d\n", col, row, old, value);
        }
    }

    destroy_spreadsheet(spreadsheet);

    return 0;
}