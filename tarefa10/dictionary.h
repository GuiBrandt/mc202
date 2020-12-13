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

/**
 * @brief Aloca e inicializa um dicionário vazio.
 * 
 * @return dicionário alocado e inicializado.
 */
dict* make_dict();

/**
 * @brief Insere um elemento em um dicionário.
 * 
 * @param d o dicionário.
 * @param word a palavra.
 */
void dict_insert(dict* d, const char* word);

color dict_query(dict* d, const char* word);

void destroy_dict(dict* dict);
