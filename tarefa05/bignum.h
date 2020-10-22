/**
 * @file bignum.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Declaração do tipo abstrato de dados para um número grande.
 */

#ifndef __LISTA_H__
#define __LISTA_H__

/**
 * @brief Estrutura opaca para a representação interna de um número grande.
 * 
 * Internamente é implementado como uma lista ligada.
 */
typedef struct _bignum_data bignum_data;

/**
 * @brief Estrutura para um número grande.
 */
typedef struct {
    bignum_data* internal; /** Sentinela */
} bignum;

/**
 * @brief Códigos de resultado de operação. 
 */
typedef enum {
    SUCCESS,                /** Sucesso */
    FAIL_OOM,               /** Sem memória */
    FAIL_DIVIDE_BY_ZERO     /** Divisão por zero */
} result_code;

/**
 * @brief Inicializa um número grande.
 * 
 * Essa função deve ser chamada antes de qualquer uso de um número grande.
 * Caso contrário, o comportamento do ADT é indeterminado.
 * 
 * Complexidade: O(1) tempo/espaço.
 * 
 * @param ptr Ponteiro para um número grande.
 * 
 * @note A função recebe um ponteiro para um bignum ao invés de retornar
 *       para que seja possível inicializar números grandes na pilha se
 *       desejável.
 * 
 * @return 1 em caso de sucesso, 0 em caso de falha (e.g. sem memória)
 */
result_code bignum_init(bignum* ptr);

/**
 * @brief Soma dois números grandes, modifica o primeiro.
 * 
 * @param lhs lado esquerdo da soma (recebe o resultado).
 * @param rhs lado direito da soma.
 * 
 * @return 1 em caso de sucesso, 0 em caso de falha (e.g. sem memória)
 */
result_code bignum_add(bignum* lhs, const bignum* rhs);

/**
 * @brief Subtrai um número grande de outro, modifica o minuendo.
 * 
 * @param lhs minuendo (recebe o resultado).
 * @param rhs subtraendo.
 * 
 * @return 1 em caso de sucesso, 0 em caso de falha (e.g. sem memória)
 */
result_code bignum_subtract(bignum* lhs, const bignum* rhs);


/**
 * @brief Multiplica dois números grandes, alterando o primeiro.
 * 
 * @param lhs lado esquerdo da multiplicação (recebe o resultado).
 * @param rhs lado direito da multiplicação.
 * 
 * @return 1 em caso de sucesso, 0 em caso de falha (e.g. sem memória)
 */
result_code bignum_multiply(bignum* lhs, const bignum* rhs);

/**
 * @brief Multiplica dois números grandes, alterando o primeiro.
 * 
 * @param lhs lado esquerdo da multiplicação (recebe o resultado).
 * @param rhs lado direito da multiplicação.
 * 
 * @return 1 em caso de sucesso, 0 em caso de falha (e.g. sem memória)
 *         e -1 em caso de divisão por 0.
 */
result_code bignum_divide(bignum* lhs, const bignum* rhs);

#endif // __LISTA_H__
