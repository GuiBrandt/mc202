/**
 * @file one_off_index.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT de dicionário com distância de edição 1.
 */

#ifndef __ONEOFF_INDEX_H__
#define __ONEOFF_INDEX_H__

#include <stdbool.h>

/**
 * @brief Tipo opaco do dicionário. 
 */
typedef struct one_off_index one_off_index;

/**
 * @brief Aloca e inicializa um dicionário vazio.
 * 
 * @return dicionário alocado e inicializado.
 */
one_off_index* make_oneoff();

/**
 * @brief Insere um elemento em um dicionário.
 * 
 * @param dict o dicionário.
 * @param word a palavra.
 */
void oneoff_insert(one_off_index* dict, const char* word);

/**
 * @brief Busca por um elemento no dicionário.
 * 
 * @param dict o dicionário.
 * @param word a palavra.
 * 
 * @return true caso exista uma palavra com distância de edição 1 da palavra
 *         dada no dicionário, false caso contrário.
 */
bool oneoff_query(one_off_index* dict, const char* word);

/**
 * @brief Libera os recursos alocados para um dicionário.
 * 
 * @param dict o dicionário.
 */
void destroy_oneoff(one_off_index* dict);

#endif // __ONEOFF_INDEX_H__
