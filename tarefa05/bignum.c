/**
 * @file bignum.c
 * @author Guilherme G. Brandt (235970)
 * @brief Implementação do ADT de número bem grande.
 * 
 * A implementação usa uma lista ligada onde cada elemento da lista é um
 * número por si só. Por questão de eficiência, os itens são números inteiros.
 * 
 * Na prática, isso funciona como um número em base 10^9 onde cada "dígito"
 * é um elemento na lista. A natureza posicional do sistema de representação
 * numérica ainda se aplica, e nós mais próximos da cabeça são menos
 * significativos.
 * 
 * Por exemplo, a lista 0 -> 1 tem valor 0x1 + 1x10^9 = 1000000000.
 * 
 * A base 10^9 foi escolhida porque é sempre possível armazenar a soma e o
 * produto de dois números de menores que 10^9 em 64 bits. Além disso, uma
 * potência de 10 facilita a leitura de um número a partir de uma string em
 * base 10.
 * 
 * O espaço usado por essa implementação para representar um número grande N é
 * O(log N), proporcional ao número de dígitos do número na base numérica
 * usada.
 */

#include "bignum.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

//============================================================================
// Definições específicas da implementação
//============================================================================

/**
 * @brief Tipo numérico para um item no número grandão.
 * 
 * Na prática, isso funciona como se cada item na lista ligada fosse um dígito
 * no número grandão, representado em base 18446744073709551616.
 */
typedef unsigned long long bignum_item;

/**
 * @brief Estrutura de nó da lista para número grande. 
 */
struct _bignum_data {
    bignum_item data;               /** Valor númerico na posição atual */
    struct _bignum_data* next;      /** Ponteiro para o próximo nó */
};

typedef struct _bignum_data* node_ptr;

/**
 * @brief Aloca e inicializa um nó.
 * 
 * @return ponteiro para o nó aloca e NULL em caso de falha.
 */
inline static node_ptr new_node() __attribute_malloc__;

inline static node_ptr new_node() {
    node_ptr node = (node_ptr) malloc(sizeof(struct _bignum_data));

    if (node == NULL)
        return NULL;

    node->data = 0;
    node->next = NULL;

    return node;
}

//============================================================================
// Utilitários
//============================================================================

/**
 * @brief Número máximo de dígitos na base decimal para um único nó da lista.
 */
#define ITEM_MAX 1000000000U

//============================================================================
// Implementações (Contrato)
//============================================================================

result_code bignum_init(bignum* dest) {
    assert(dest != NULL);

    dest->internal = new_node();

    if (dest->internal == NULL)
        return FAIL_OOM;
    
    return SUCCESS;
}

#include <stdio.h>

result_code bignum_parse(bignum* dest, const char* str) {
    assert(dest != NULL);
    assert(dest->internal != NULL);
    assert(dest->internal->data == 0);
    assert(dest->internal->next == NULL);
    assert(str != NULL);
    
    node_ptr current = dest->internal;

    // Lemos a string de trás para a frente, porque números menos
    // significativos devem estar no começo da lista. Isso facilita
    // crescer a lista quando estourarmos o limite de um nó.
    // Também mantemos um contador para a potência de 10 sendo aplicada.
    int pow = 1;
    for (int i = strlen(str) - 1; i >= 0; i--, pow *= 10) {
        // Se adicionar um dígito estouraria nosso limite, criamos um
        // novo nó para armazenar o resto.
        if (current->data >= ITEM_MAX / 10) {
            current = current->next = new_node();
            if (current == NULL)
                return FAIL_OOM;

            pow = 1; // Redefinimos a potência de 10 para o próximo nó
        }

        int digit = str[i] - '0';
        current->data += digit * pow;
    }

    return SUCCESS;
}

void bignum_destroy(bignum* dest) {
    node_ptr current = dest->internal, next;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}

int main() {
    bignum big;
    bignum_init(&big);

    bignum_parse(&big, "1234567891011121311415161718192021222324252627282930");

    node_ptr current = big.internal;
    while (current) {
        printf("%llu -> ", current->data);
        current = current->next;
    }

    printf("<END>\n");

    bignum_destroy(&big);
}
