/**
 * @file tree_multiset.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definições do ADT de multiset usado na tarefa.
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa08.html
 * 
 * O ADT fornece funções para inserção e contagem de elementos com um certo
 * valor no conjunto. 
 */

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Tipo de elemento armazenado no conjunto. 
 */
typedef uint64_t element_t;

/**
 * @brief Estrutura opaca do conjunto. 
 */
typedef struct tree_multiset tree_multiset;

/**
 * @brief Aloca e inicializa um novo multiset vazio.
 * 
 * @return ponteiro para o multiset alocado ou NULL em caso de falha.
 */
tree_multiset* multiset_init() __attribute_malloc__;

/**
 * @brief Insere um valor em um multiset dado.
 * 
 * A complexidade dessa operação é garantida ser O(log n) amortizado.
 * 
 * @param multiset o conjunto no qual inserir o valor.
 * @param elem o valor a inserir.
 * 
 * @return um ponteiro para o multiset modificado. Pode ser diferente do
 *         parâmetro fornecido, e pode ser NULL em caso de falha.
 */
tree_multiset* multiset_insert(tree_multiset* multiset, element_t elem);

/**
 * @brief Conta o número de ocorrências de um valor em um multiset dado.
 * 
 * A complexidade dessa operação é garantida ser O(log n) amortizado.
 * 
 * @param multiset o conjunto em questão.
 * @param elem o elemento sendo contado.
 * 
 * @return o número de ocorrências do valor no multiset (0 caso não exista).
 */
size_t multiset_count(tree_multiset* multiset, element_t elem);

/**
 * @brief Libera os recursos alocados para um multiset.
 * 
 * @param multiset o conjunto.
 */
void multiset_destroy(tree_multiset* multiset);
