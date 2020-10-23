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
 * @brief Valor máximo para um único item na lista, i.e., a base numérica
 *        adotada para representação dos números. 
 * 
 * Este valor não pode ser maior que 1000000000 porque é necessário que o
 * produto de quaisquer dois itens caiba em 64-bits para o processo de
 * multiplicação.
 */
#define ITEM_MAX 1000000000U

#ifdef NDEBUG
#define assert_valid ((void)0)
#else
void assert_valid(bignum* ptr) {
    assert(ptr != NULL);
    assert(ptr->internal != NULL);
}
#endif

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

result_code bignum_copy(bignum* dest, const bignum* source) {
    assert_valid(dest);
    assert(dest->internal->next == NULL);
    assert_valid(source);
    
    node_ptr current_dest = dest->internal,
             current_source = source->internal;

    current_dest->data = current_source->data;

    while (current_source->next) {
        current_source = current_source->next;

        current_dest = current_dest->next = new_node();
        if (current_dest == NULL)
            return FAIL_OOM;

        current_dest->data = current_source->data;
    }

    return SUCCESS;
}

void bignum_destroy(bignum* dest) {
    assert_valid(dest);

    node_ptr current = dest->internal, next;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}

result_code bignum_parse(bignum* dest, const char* str) {
    assert_valid(dest);
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
        if (pow >= ITEM_MAX) {
            current = current->next = new_node();
            if (current == NULL)
                return FAIL_OOM;

            pow = 1; // Redefinimos a potência de 10 para o próximo nó
        }

        assert('0' <= str[i] && str[i] <= '9');

        int digit = str[i] - '0';
        current->data += digit * pow;
    }

    return SUCCESS;
}

int bignum_cmp(const bignum* lhs, const bignum* rhs) {
    assert_valid(lhs);
    assert_valid(rhs);

    node_ptr current_lhs = lhs->internal,
             current_rhs = rhs->internal;

    // Compara cada item das listas. A ideia é manter o resultado do maior
    // item até o momento (uma vez que ele é sempre o mais significativo) que
    // não foi igual nos dois números.
    int current_cmp = 0;
    while (current_lhs != NULL && current_rhs != NULL) {
        if (current_lhs->data < current_rhs->data) {
            current_cmp = -1;
        } else if (current_lhs->data > current_rhs->data) {
            current_cmp = 1;
        }

        current_lhs = current_lhs->next;
        current_rhs = current_rhs->next;
    }

    // As listas ainda podem ter tamanhos diferentes, então temos que tratar
    // esse caso.
    if (current_lhs == NULL && current_rhs != NULL) {
        return -1;
    } else if (current_lhs != NULL && current_rhs == NULL) {
        return 1;
    }

    // Se as listas tinham o mesmo tamanho, a comparação dos últimos itens
    // diferentes nelas dão o resultado.
    return current_cmp;
}

result_code bignum_add(bignum* lhs, const bignum* rhs) {
    assert_valid(lhs);
    assert_valid(rhs);

    node_ptr current_lhs = lhs->internal,
             current_rhs = rhs->internal;

    while (current_rhs) {
        // Essa soma sempre é segura, pois garantimos que os dados são menores
        // que 10^9, e portanto sua soma sempre cabe no tipo inteiro usado.
        current_lhs->data += current_rhs->data;

        // Por outro lado, temos que considerar que podemos estourar o limite
        // e temos que "carregar" o que sobrou para os próximos nós.
        node_ptr current_carry = current_lhs;
        while (current_carry->data >= ITEM_MAX) {

            int carry = current_carry->data / ITEM_MAX;
            current_carry->data %= ITEM_MAX;

            // Criamos um novo nó caso não tenhamos o suficiente
            if (current_carry->next == NULL) {
                current_carry->next = new_node();
                if (current_carry->next == NULL)
                    return FAIL_OOM;
            }

            current_carry->next->data += carry;
            current_carry = current_carry->next;
        }

        current_lhs = current_lhs->next;
        current_rhs = current_rhs->next;
    }

    return SUCCESS;
}

// Implementação de subtração assumindo que lhs >= rhs.
result_code bignum_subtract_basic(bignum* lhs, const bignum* rhs) {
    node_ptr current_lhs = lhs->internal,
             current_rhs = rhs->internal;

    while (current_rhs) {

        // Se o lado direito é maior que o esquerdo, temos que "emprestar" um
        // valor das casas acima para fazer a subtração.
        if (current_rhs->data > current_lhs->data) {

            current_lhs->data += ITEM_MAX;
            current_lhs->data -= current_rhs->data;

            // Procuramos um valor não nulo acima e replicamos o passo de
            // "emprestar" para cada nó no caminho, até encontrarmos um que
            // possamos decrementar.
            node_ptr current_carry = current_lhs->next;
            while (current_carry->data == 0) {
                current_carry->data = ITEM_MAX - 1;
                current_carry = current_carry->next;
            }

            current_carry->data--;

        // Se não, podemos simplesmente subtrair e tudo fica bem
        } else {
            current_lhs->data -= current_rhs->data;
        }

        current_lhs = current_lhs->next;
        current_rhs = current_rhs->next;
    }

    return SUCCESS;
}

result_code bignum_subtract(bignum* lhs, const bignum* rhs) {
    assert_valid(lhs);
    assert_valid(rhs);

    // Se lhs < rhs, temos que trocar os números para calcular a diferença
    // absoluta.
    if (bignum_cmp(lhs, rhs) < 0) {

        // Criamos um auxiliar para copiar o rhs para a operação. Daria para
        // fazer destruíndo o rhs, mas assim parece mais elegante apesar de
        // menos eficiente (a complexidade do algoritmo continua a mesma, de
        // toda forma).
        bignum aux;

        result_code result;
        if ((result = bignum_init(&aux)) != SUCCESS)
            return result;

        if ((result = bignum_copy(&aux, rhs)) == SUCCESS) {
            node_ptr swap = lhs->internal;
            lhs->internal = aux.internal;
            aux.internal = swap;

            result = bignum_subtract_basic(lhs, &aux);
        }

        bignum_destroy(&aux);
        return result;

    } else {
        return bignum_subtract_basic(lhs, rhs);
    }
}

#include <stdio.h>

int main() {
    bignum big1, big2;
    bignum_init(&big1);
    bignum_init(&big2);

    bignum_parse(&big1, "1000000000000000000");
    bignum_parse(&big2, "1000000000000000000");

    node_ptr current = big1.internal;
    while (current) {
        printf("%llu -> ", current->data);
        current = current->next;
    }
    printf("<END>\n");

    bignum_subtract(&big2, &big1);
    bignum_add(&big2, &big1);

    current = big2.internal;
    while (current) {
        printf("%llu -> ", current->data);
        current = current->next;
    }
    printf("<END>\n");

    bignum_destroy(&big1);
    bignum_destroy(&big2);
}
