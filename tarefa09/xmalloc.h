/**
 * @file xmalloc.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição da função malloc adaptada para tratar OOM.
 */

#include <stddef.h>

/**
 * @brief Aloca memória ou termina o programa com código de erro -1 em caso de
 *        falha.
 * 
 * @param size tamanho desejado.
 * 
 * @return ponteiro para a memória alocada.
 */
void* xmalloc(size_t size);
