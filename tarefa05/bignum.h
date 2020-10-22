/**
 * @file bignum.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Declaração do tipo abstrato de dados para um número grande.
 */

#ifndef __LISTA_H__
#define __LISTA_H__

#include <stdlib.h>

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
    FAIL_DIVIDE_BY_ZERO,    /** Divisão por zero */
    FAIL_STRING_OVERFLOW    /** Número grande demais para o buffer dado */
} result_code;

/**
 * @brief Inicializa um número grande.
 * 
 * Essa função deve ser chamada antes de qualquer uso de um número grande.
 * Caso contrário, o comportamento do ADT é indeterminado.
 * 
 * Complexidade: O(1)
 * 
 * @param dest Ponteiro para um número grande.
 * 
 * @note A função recebe um ponteiro para um bignum ao invés de retornar
 *       para que seja possível inicializar números grandes na pilha se
 *       desejável.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code bignum_init(bignum* dest);

/**
 * @brief Converte uma string em um número bem grande.
 * 
 * Complexidade: O(|str|)
 * 
 * @param dest Ponteiro para um número grande pré-inicializado.
 * @param str String a ser convertida.
 * 
 * @note A função assume que o ponteiro de destino acabou de ser inicializado,
 *       isto é, o ponteiro é válido mas não contém nenhum valor. O
 *       comportamento é indeterminado caso contrário.
 * 
 * @return SUCCESS ou FAIL_OOM. 
 */
result_code bignum_parse(bignum* dest, const char* str);

/**
 * @brief Converte um número bem grande em uma string.
 * 
 * Complexidade: O(log N), N = o número bem grande.
 * 
 * @param str String de destino.
 * @param len Tamanho máximo da string (contando '\0').
 * @param source Número bem grande.
 * 
 * @return SUCCESS ou FAIL_STRING_OVERFLOW, caso o número não caiba na string
 *         passada.
 */
result_code bignum_sprintf(char* str, size_t len, const bignum* source);

/**
 * @brief Libera os recursos criados para um número grande.
 * 
 * Complexidade: O(n)
 * 
 * @param dest Ponteiro para um número grande.
 * 
 * @note Após chamar a função, o objeto fica **inválido** até a próxima
 *       chamada a `bignum_init`.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
void bignum_destroy(bignum* dest);

/**
 * @brief Soma dois números grandes, modifica o primeiro.
 * 
 * Complexidade: O(log N) tempo, onde N = maior número entre os argumentos.
 * 
 * @param lhs lado esquerdo da soma (recebe o resultado).
 * @param rhs lado direito da soma.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code bignum_add(bignum* lhs, const bignum* rhs);

/**
 * @brief Subtrai um número grande de outro, modifica o minuendo.
 * 
 * Complexidade: O(log N) tempo, onde N = maior número entre os argumentos.
 * 
 * @param lhs minuendo (recebe o resultado).
 * @param rhs subtraendo.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code bignum_subtract(bignum* lhs, const bignum* rhs);


/**
 * @brief Multiplica dois números grandes, alterando o primeiro.
 * 
 * Complexidade: O(log N) tempo, onde N = maior número entre os argumentos.
 * 
 * @param lhs lado esquerdo da multiplicação (recebe o resultado).
 * @param rhs lado direito da multiplicação.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code bignum_multiply(bignum* lhs, const bignum* rhs);

/**
 * @brief Multiplica dois números grandes, alterando o primeiro.
 * 
 * Complexidade: O(log N) tempo, onde N = maior número entre os argumentos.
 * 
 * @param lhs lado esquerdo da multiplicação (recebe o resultado).
 * @param rhs lado direito da multiplicação.
 * 
 * @return SUCCESS, FAIL_OOM ou FAIL_DIVIDE_BY_ZERO caso rhs == 0.
 */
result_code bignum_divide(bignum* lhs, const bignum* rhs);

#endif // __LISTA_H__
