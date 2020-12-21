/**
 * @file spreadsheet.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de planilha.
 */

#include "spreadsheet.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>

#include "xmalloc.h"
#include "expressions.h"

//=============================================================================
// DEFINIÇÕES
//=============================================================================

/**
 * @brief Enumeração de estado de validação interno de uma célula na planilha.
 * 
 * Os valores UNCHECKED e CHECKING são utilizados exclusivamente durante a
 * detecção de ciclos.
 * 
 * Ao final do procedimento de validação, cada célula deve ter estado VALID ou
 * CYCLIC, correspondendo a células sem ciclos e com ciclos respectivamente. 
 */
typedef enum validation_status {
    UNCHECKED = 0,
    CHECKING,
    VALID,
    CYCLIC
} validation_status;

/**
 * @brief Estrutura de célula da planilha. 
 */
typedef struct cell {
    expression_t* expression;
    validation_status status;
} cell;

/**
 * @brief Estrutura interna da planilha.
 */
struct spreadsheet {
    size_t n_rows;
    size_t n_cols;
    cell** cells;
};

//=============================================================================
// IMPLEMENTAÇÃO (Auxiliares)
//=============================================================================

/**
 * @brief Aloca e inicializa os recursos necessários para uma planilha.
 * 
 * Esta função interrompe o programa em caso de falha.
 * 
 * @param rows número de linhas.
 * @param cols número de colunas.
 * 
 * @return ponteiro para a planilha alocada. 
 */
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

/**
 * @brief Lê uma linha de dados de um arquivo.
 * 
 * @param file ponteiro para o arquivo.
 * @param row ponteiro de saída para as células da linha.
 * @param cols número de colunas.
 */
void read_row(FILE* file, size_t cols, cell* row) {
    char* cell_str;

    int read = fscanf(file, " %m[^,] ", &cell_str);

    int i = 0;
    do {
        assert(read == 1);

        row[i++].expression = parse_expr(cell_str);
        free(cell_str);

        read = fscanf(file, " , %m[^,\n] ", &cell_str);
    } while (i < cols);
}

/**
 * @brief Obtém um ponteiro para uma célula na planilha.
 * 
 * @param s a planilha.
 * @param column nome da coluna (A-Z).
 * @param row número da linha.
 * 
 * @return ponteiro para a célula correspondente às posições dadas.
 */
cell* spreadsheet_cell(spreadsheet_t* s, char column, size_t row) {
    assert('A' <= column && column <= 'A' + s->n_cols);
    assert(0 < row && row <= s->n_rows);

    return &s->cells[row - 1][column - 'A'];
}

/**
 * @brief Marca dependências cíclicas no digrafo de dependências de uma célula
 *        da planilha.
 * 
 * Ao fim da função, todas as células no digrafo de dependências da célula dada
 * são garantidos estarem marcados como VALID ou CYCLIC.
 * 
 * @param s a planilha.
 * @param c 
 */
void check_cycles(spreadsheet_t* s, cell* c) {
    // Se chegamos à célula novamente enquanto checávamos ela, temos um ciclo.
    if (c->status == CHECKING) {
        c->status = CYCLIC;
    }
    
    // Se a célula estiver marcada como VALID ou CYCLIC, não precisamos checar
    // ela de novo.
    if (c->status != UNCHECKED) {
        return;
    }

    // Assumimos que cada célula referencia no máximo até 16 outras células; é
    // uma limitação artificial que poderia ser contornada de diversas formas,
    // por exemplo, alocando o vetor no heap e crescendo ele dinamicamente.
    // Para o contexto da tarefa, este tamanho fixo é mais que suficiente.
    //
    // Além disso, convenhamos, uma célula de excel que faz referência pra 16
    // outras já não deveria existir, quem dirá mais que isso.
    reference_t neighbors[16];
    int deps = expr_dependencies(c->expression, 16, neighbors);

    c->status = CHECKING;

    for (int i = 0; i < deps; i++) {
        reference_t neighbor = neighbors[i];
        
        cell* nc = spreadsheet_cell(s, neighbor.col, neighbor.row);
        check_cycles(s, nc);

        // Se qualquer um dos vizinhos (dependências) tiver um ciclo, então
        // esta célula tem um cíclo, e podemos terminar a verificação. 
        if (nc->status == CYCLIC) {
            c->status = CYCLIC;
            break;
        }
    }

    // Se o estado não foi alterado para CYCLIC, então não existem ciclos no
    // digrafo.
    if (c->status == CHECKING) {
        c->status = VALID;
    }

    assert(c->status == CYCLIC || c->status == VALID);
}

/**
 * @brief Obtém a expressão de uma célula dada por uma referência na planilha.
 * 
 * @param s a planilha.
 * @param ref a referência (coluna/linha).
 * 
 * @return a expressão contida na célula.
 */
expression_t* spreadsheet_expr(spreadsheet_t* s, reference_t ref) {
    cell* c = spreadsheet_cell(s, ref.col, ref.row);

    if (c->status == VALID) {
        return c->expression;
    } else {
        assert(c->status == CYCLIC);
        return NULL;
    }
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

spreadsheet_t* read_spreadsheet(FILE* file, size_t rows, size_t cols) {
    spreadsheet_t* spreadsheet = make_spreadsheet(rows, cols);
    
    for (int i = 0; i < rows; i++) {
        read_row(file, cols, spreadsheet->cells[i]);
    }

    // Checamos por ciclos para cada célula após ler a planilha por completo.
    // Só temos que fazer este procedimento uma única vez quando lemos a
    // planilha, visto que todas as células com dependências são imutáveis.
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cell* c = &spreadsheet->cells[i][j];

            if (c->status != UNCHECKED) {
                continue;
            }

            check_cycles(spreadsheet, c);
        }
    }

    return spreadsheet;
}

int spreadsheet_eval(spreadsheet_t* spreadsheet, char column, size_t row) {
    expression_t* expr = spreadsheet_cell(spreadsheet, column, row)->expression;

    return eval_expr(
        expr,
        (void*) spreadsheet,
        (resolve_fn_t) spreadsheet_expr
    );
}

int spreadsheet_update(
    spreadsheet_t* spreadsheet,
    char column,
    size_t row,
    int value
) {
    cell* c = spreadsheet_cell(spreadsheet, column, row);
    destroy_expr(c->expression);

    c->expression = const_expr(value);
    return value;
}

void destroy_spreadsheet(spreadsheet_t* spreadsheet) {
    for (int i = 0; i < spreadsheet->n_rows; i++) {
        for (int j = 0; j < spreadsheet->n_cols; j++) {
            destroy_expr(spreadsheet->cells[i][j].expression);
        }

        free(spreadsheet->cells[i]);
    }

    free(spreadsheet->cells);
    free(spreadsheet);
}
