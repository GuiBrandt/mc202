/**
 * @file dictionary.h
 * @author Guilherme G. Brandt (gui.g.bandt@gmail.com)
 * 
 * @brief Declaração do ADT de dicionário.
 *
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa10.html 
 */

typedef struct dict dict;

typedef enum color {
    GREEN,
    YELLOW,
    RED
} color;

dict* make_dict();
void dict_insert(const char* word);
color dict_query(const char* word);
void destroy_dict(dict* dict);
