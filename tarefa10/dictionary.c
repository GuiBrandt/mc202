/**
 * @file dictionary.c
 * @author Guilherme G. Brandt (gui.g.bandt@gmail.com)
 * 
 * @brief Implementação do ADT de dicionário usando Hash table e BK-Tree.
 *
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa10.html 
 */

#include "dictionary.h"

#include <stddef.h>

#include "one_off_index.h"

typedef struct hash_table {
    size_t size;
    const char* data;
} hash_table;

struct dict {
    hash_table exact_index;
    one_off_index* approx_index;
};

dict* make_dict() {
    
}

void dict_insert(const char* word) {

}

color dict_query(const char* word) {

}

void destroy_dict(dict* dict) {

}
