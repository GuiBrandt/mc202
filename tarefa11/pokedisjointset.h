/**
 * @file pokedisjointset.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT de união-busca (disjoint-set) usado no algoritmo de
 *        caminho de menor aresta máxima.
 * 
 * Além das operações usuais de união e busca, o ADT define funções para
 * "marcação" de conjuntos, onde um conjunto pode ser marcado e testado por
 * marcação, e a operação de união preserva a marcação de um conjunto. i.e. se
 * um conjunto marcado é mesclado com outro não marcado, o conjunto resultante
 * será marcado.
 */

#ifndef __POKEDISJOINTSET_H__
#define __POKEDISJOINTSET_H__

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Estrutura opaca de união-busca. 
 */
typedef struct disjoint_set disjoint_set_t;

/**
 * @brief Constrói um disjoint-set com uma capacidade dada.
 * 
 * @param capacity a capacidade da estrutura.
 * 
 * @return um disjoint-set inicializado onde cada elemento faz parte de um
 *         conjunto unitário.
 */
disjoint_set_t* make_disjoint_set(size_t capacity);

/**
 * @brief Busca o representante do conjunto de um elemento.
 * 
 * @param ds estrutura de união-busca.
 * @param element o elemento buscado.
 * 
 * @return índice do representante do conjunto que contém o elemento. 
 */
int disjoint_set_find(disjoint_set_t* ds, int element);

/**
 * @brief Marca o conjunto de um elemento.
 * 
 * @param ds estrutura de união-busca.
 * @param element o elemento sendo marcado.
 */
void disjoint_set_mark(disjoint_set_t* ds, int element);

/**
 * @brief Verifica se o conjunto de um elemento está marcado.
 * 
 * @param ds estrutura de união-busca.
 * @param element o elemento sendo buscado.
 * 
 * @return true caso o conjunto esta marcado ou false caso contrário.
 */
bool disjoint_set_marked(disjoint_set_t* ds, int element);

/**
 * @brief Mescla os conjuntos de dois elementos.
 * 
 * @param ds estrutura de união-busca.
 * @param a um elemento.
 * @param b outro elemento.
 */
void disjoint_set_merge(disjoint_set_t* ds, int a, int b);

/**
 * @brief Libera os recursos alocados para uma estrutura de união-busca.
 * 
 * @param ds estrutura de união-busca.
 */
void destroy_disjoint_set(disjoint_set_t* ds);

#endif // __POKEDISJOINTSET_H__
