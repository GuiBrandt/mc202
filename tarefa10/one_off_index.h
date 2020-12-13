/**
 * @file one_off_index.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT de dicionário com distância de edição 1.
 */

#ifndef __ONEOFF_INDEX_H__
#define __ONEOFF_INDEX_H__

#include <stdbool.h>

typedef struct one_off_index one_off_index;

one_off_index* make_oneoff();
void oneoff_insert(one_off_index* dict, const char* word);
bool oneoff_query(one_off_index* dict, const char* word);
void destroy_oneoff(one_off_index* dict);

#endif // __ONEOFF_INDEX_H__
