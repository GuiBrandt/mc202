#ifndef __SPREADSHEET_H__
#define __SPREADSHEET_H__

#include <stdio.h>
#include <stddef.h>

typedef struct spreadsheet spreadsheet_t;

spreadsheet_t* read_spreadsheet(FILE* file);
int spreadsheet_eval(char column, size_t row);
int spreadsheet_update(char column, size_t row, int value);
void destroy_spreadsheet(spreadsheet_t* spreadsheet);

#endif // __SPREADSHEET_H__
