/**
 * @file dictionary.h
 * @author Guilherme G. Brandt (gui.g.bandt@gmail.com)
 * 
 * @brief Declaração do ADT de dicionário.
 *
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa10.html 
 */

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

/**
 * @brief Tipo opaco do dicionário. 
 */
typedef struct dict dict;

/**
 * @brief Enumeração de cores.
 * 
 * GREEN indica uma palavra presente no dicionário.
 * 
 * YELLOW indica uma palavra presente no dicionário dada uma edição.
 * 
 * RED indica uma palavra que necessitaria de duas ou mais edições para bater
 * com uma palavra no dicionário.
 */
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

/**
 * @brief Busca por um elemento no dicionário.
 * 
 * @param d o dicionário.
 * @param word a palavra.
 * 
 * @return GREEN caso a palavra exista exatamente como dado no dicionário,
 *         YELLOW caso a palvra exista com uma edição no dicionário, e RED caso
 *         contrário.
 */
color dict_query(dict* d, const char* word);

/**
 * @brief Libera os recursos alocados para um dicionário.
 * 
 * @param dict o dicionário.
 */
void destroy_dict(dict* dict);

#endif // __DICTIONARY_H__
