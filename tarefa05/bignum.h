/**
 * @file bignum.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Declaração do tipo abstrato de dados para um número grande.
 */

#ifndef __LISTA_H__
#define __LISTA_H__

#include <stddef.h> // size_t

/**
 * @brief Estrutura opaca para a representação interna de um número grande.
 * 
 * Internamente é implementado como uma lista ligada.
 */
struct _bignum_data;

/**
 * @brief Estrutura para um número bem grande.
 */
typedef struct {
    struct _bignum_data* internal; /** Sentinela */
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
 * @brief Inicializa um número grande com valor 0.
 * 
 * Essa função deve ser chamada antes de qualquer uso de um número grande.
 * Caso contrário, o comportamento do ADT é indeterminado.
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
 * @brief Copia um número grande para outro número grande.
 * 
 * @note A função assume que o ponteiro de destino acabou de ser inicializado,
 *       isto é, o ponteiro é válido mas não contém nenhum valor. O
 *       comportamento é indeterminado caso contrário.
 * 
 * @param dest Ponteiro de destino.
 * @param source Ponteiro para o número grande a ser copiado.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code bignum_copy(bignum* dest, const bignum* source);

/**
 * @brief Libera os recursos criados para um número grande.
 * 
 * @param dest Ponteiro para um número grande.
 * 
 * @note Após chamar a função, o objeto fica **inválido** até a próxima
 *       chamada a `bignum_init`. Observe que esta função **não libera**
 *       o ponteiro passado.
 */
void bignum_destroy(bignum* dest);

/**
 * @brief Converte uma string em um número bem grande.
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
 * @param str String de destino.
 * @param len Tamanho máximo da string (contando '\0').
 * @param source Número bem grande.
 * 
 * @return SUCCESS ou FAIL_STRING_OVERFLOW, caso o número não caiba na string
 *         passada.
 */
result_code bignum_sprintf(char* str, size_t len, const bignum* source);

/**
 * @brief Compara dois números grandes.
 * 
 * @param lhs Lado esquerdo da comparação.
 * @param rhs Lado direito da comparação.
 * 
 * @return -1 se lhs < rhs, 0 se forem iguais e 1 caso contrário.
 */
int bignum_cmp(const bignum* lhs, const bignum* rhs);

/**
 * @brief Soma dois números grandes, modifica o primeiro.
 * 
 * @param lhs lado esquerdo da soma (recebe o resultado).
 * @param rhs lado direito da soma.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code bignum_add(bignum* lhs, const bignum* rhs);

/**
 * @brief Subtrai um número grande de outro, mantendo o resultado positivo
 * (i.e. tira o módulo).
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
 * @param lhs lado esquerdo da multiplicação (recebe o resultado).
 * @param rhs lado direito da multiplicação.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code bignum_multiply(bignum* lhs, const bignum* rhs);

/**
 * @brief Divide dois números grandes, alterando o primeiro.
 * 
 * @param lhs lado esquerdo da multiplicação (recebe o resultado).
 * @param rhs lado direito da multiplicação.
 * 
 * @return SUCCESS, FAIL_OOM ou FAIL_DIVIDE_BY_ZERO caso rhs == 0.
 */
result_code bignum_divide(bignum* lhs, const bignum* rhs);

#endif // __LISTA_H__
