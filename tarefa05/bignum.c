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
#include <alloca.h>

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
 * @brief Adiciona um próximo para um nó caso ele não tenha.
 * 
 * @param node Nó a ser estendido.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
inline static result_code extend_if_needed(node_ptr node) {
    if (node->next == NULL) {
        node->next = new_node();

        if (node->next == NULL) {
            return FAIL_OOM;
        }
    }

    return SUCCESS;
}

/**
 * @brief Valor máximo para um único item na lista, i.e., a base numérica
 *        adotada para representação dos números. 
 * 
 * Este valor não pode ser maior que 1000000000 porque é necessário que o
 * produto de quaisquer dois itens caiba em 64-bits para o processo de
 * multiplicação.
 */
#define ITEM_MAX 100000000U

#ifdef NDEBUG
#define assert_valid ((void)0)
#else
void assert_valid(const bignum* ptr) {
    assert(ptr != NULL);
    assert(ptr->internal != NULL);
}
#endif

result_code add_with_carry(node_ptr current_lhs, bignum_item n) {
    // Esta soma sempre é segura desde que N obedeça os limites de tamanho.
    current_lhs->data += n;

    // Por outro lado, temos que considerar que podemos estourar o limite
    // e temos que "carregar" o que sobrou para os próximos nós.
    node_ptr current_carry = current_lhs;
    while (current_carry->data >= ITEM_MAX) {

        int carry = current_carry->data / ITEM_MAX;
        current_carry->data %= ITEM_MAX;

        // Criamos um novo nó caso não tenhamos o suficiente
        result_code result;
        if ((result = extend_if_needed(current_carry)) != SUCCESS)
            return result;

        current_carry->next->data += carry;
        current_carry = current_carry->next;
    }

    return SUCCESS;
}

result_code add_at_node(node_ptr current_lhs, const bignum* rhs) {
    for (
        node_ptr current_rhs = rhs->internal;
        current_rhs != NULL;
        current_lhs = current_lhs->next, current_rhs = current_rhs->next
    ) {
        bignum_item n = current_rhs->data;
        result_code result;

        // Essa soma sempre é segura, pois garantimos que os dados são menores
        // que 10^9, e portanto sua soma sempre cabe no tipo inteiro usado.
        if ((result = add_with_carry(current_lhs, n)) != SUCCESS)
            return result;

        if (current_rhs->next != NULL) {
            result_code result;
            if ((result = extend_if_needed(current_lhs)) != SUCCESS)
                return result;
        }
    }

    return SUCCESS;
}

// Implementação de subtração assumindo que lhs >= rhs.
void subtract_base(bignum* lhs, const bignum* rhs) {
    node_ptr current_lhs = lhs->internal,
             prev_lhs = NULL,
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
            node_ptr current_carry = current_lhs->next,
                     prev = current_lhs;

            while (current_carry->data == 0) {
                current_carry->data = ITEM_MAX - 1;
                prev = current_carry;
                current_carry = current_carry->next;
            }

            // Se formos deixar algum zero à esquerda, liberamos o nó
            // correspondente.
            if (current_carry->data == 1 && current_carry->next == NULL) {
                prev->next = NULL;
                free(current_carry);
            } else {
                current_carry->data--;
            }

        // Se não, podemos simplesmente subtrair e tudo fica bem
        } else {
            current_lhs->data -= current_rhs->data;

            // Se formos deixar algum zero à esquerda, liberamos o nó
            // correspondente e paramos a iteração.
            if (prev_lhs && current_lhs->data == 0 && current_lhs->next == NULL) {
                prev_lhs->next = NULL;
                free(current_lhs);
                break;
            }
        }

        prev_lhs = current_lhs;
        current_lhs = current_lhs->next;
        current_rhs = current_rhs->next;
    }
}

// Multiplica um bignum por um único nó, e soma o resultado a um auxiliar
result_code multiply_partial(
    node_ptr current_aux,
    node_ptr fixed,
    const bignum* lhs
) {
    if (fixed->data == 0)
        return SUCCESS;

    result_code result = SUCCESS;

    // Criamos mais um bignum auxiliar, dessa vez para armazenar os
    // produtos parciais referentes à multiplicação de cada dígito em rhs
    // pelo valor completo de lhs.
    bignum summand;
    if ((result = bignum_init(&summand)) != SUCCESS)
        return result;

    for (
        node_ptr current_lhs = lhs->internal,
                 current_summand = summand.internal;
        current_lhs != NULL;
        current_lhs = current_lhs->next,
        current_summand = current_summand->next
    ) {
        bignum_item product = current_lhs->data * fixed->data;
        if ((result = add_with_carry(current_summand, product)) != SUCCESS)
            return result;

        if (current_lhs->next != NULL) {
            result_code result;
            if ((result = extend_if_needed(current_summand)) != SUCCESS) {
                bignum_destroy(&summand);
                return result;
            }
        }
    }

    // Então somamos o produto parcial ao total e nos livramos dele
    result = add_at_node(current_aux, &summand);
    bignum_destroy(&summand);

    return SUCCESS;
}

void reverse(bignum* ptr) {
    node_ptr prev = ptr->internal, current = prev->next;

    while (current != NULL) {
        node_ptr next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    ptr->internal->next = NULL;
    ptr->internal = prev;
}

// Caso simplificado da divisão, onde lhs / rhs é garantido ter exatamente um
// dígito na base usada.
// Faz busca binária nos valores possíveis para determinar o quociente.
bignum_item divide_base(bignum* lhs, const bignum* rhs) {
    bignum_item left = 0, right = ITEM_MAX - 1;

    bignum product;
    while (left < right) {
        bignum_item mid = (left + right) / 2;
        bignum_init(&product);

        node_ptr current = product.internal;
        for (node_ptr it = rhs->internal; it != NULL; it = it->next) {
            bignum_item p = it->data * mid;
            add_with_carry(current, p);

            if (it->next != NULL)
                extend_if_needed(current);

            current = current->next;
        }

        int cmp = bignum_cmp(&product, lhs);
        if (cmp > 0) {
            right = mid;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            left = right = mid + 1;
            bignum_subtract(lhs, &product);
        }

        bignum_destroy(&product);
    }

    return left - 1;
}

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

result_code bignum_sprintf(char* dest, size_t len, const bignum* source) {
    assert(dest != NULL);
    assert_valid(source);

    node_ptr current = source->internal;
    
    size_t i;
    int pow = 1;
    for (i = 0; current != NULL && i < len - 1; i++) {
        // Valor do nó atual deslocado pela potência de 10 atual.
        bignum_item q = current->data / pow;
        if (q == 0 && current->next == NULL)
            break;

        // O dígito atual é o resto da divisão do valor deslocade por 10.
        dest[i] = '0' + q % 10;

        // Aumenta a potência de dez e avança um nó caso preciso.
        pow *= 10;
        if (pow >= ITEM_MAX) {
            current = current->next;
            pow = 1;
        }
    }

    // Se chegou no final da string, acabou o espaço (não cabe nem o \0!).
    if (i == len)
        return FAIL_STRING_OVERFLOW;

    // Inverte a string, uma vez que a lista começa dos dígitos menos
    // significativos e em uma string geralmente queremos o contrário.
    for (size_t j = 0; j < i / 2; j++) {
        char swap = dest[j];
        dest[j] = dest[i - j - 1];
        dest[i - j - 1] = swap;
    }

    if (i == 0) {
        dest[i] = '0';
        i++;
    }

    dest[i] = '\0';

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

    node_ptr current_lhs = lhs->internal;
    return add_at_node(current_lhs, rhs);
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

            subtract_base(lhs, &aux);
        }

        bignum_destroy(&aux);
        return result;

    } else {
        subtract_base(lhs, rhs);
        return SUCCESS;
    }
}

result_code bignum_multiply(bignum* lhs, const bignum* rhs) {
    assert_valid(lhs);
    assert_valid(rhs);

    // Cria um bignum auxiliar para receber o resultado sem destruir o lado
    // esquerdo.
    bignum aux;

    result_code result;
    if ((result = bignum_init(&aux)) != SUCCESS)
        return result;

    // Caso especial: se lhs = 0, o retorno é 0.
    // Este caso é tratado separadamente porque é o único caso em que seria
    // possível introduzir zeros à esquerda no número, por conta do próximo
    // passo.
    if (lhs->internal->next == NULL && bignum_cmp(lhs, &aux) == 0) {
        bignum_destroy(&aux);
        return SUCCESS;
    }

    // Percorremos para elemento do lado direito para multiplicar pelos
    // elementos do lado esquerdo.
    // Também avançamos na lista do auxiliar ao mesmo tempo, equivalente à
    // multiplicação pelas potências de 10 que fazemos quando aplicamos o
    // algoritmo da multiplicação conforme multiplicamos por casas decimais
    // maiores.
    for (
        node_ptr current_rhs = rhs->internal, current_aux = aux.internal;
        current_rhs != NULL;
        current_rhs = current_rhs->next, current_aux = current_aux->next
    ) {
        result = multiply_partial(current_aux, current_rhs, lhs);

        if (result != SUCCESS) {
            bignum_destroy(&aux);
            return result;
        }

        if (current_rhs->next != NULL) {
            result_code result;
            if ((result = extend_if_needed(current_aux)) != SUCCESS) {
                bignum_destroy(&aux);
                return result;
            }
        }
    }

    // Ao final, trocamos o auxiliar com o resultado e o lhs
    node_ptr swap = lhs->internal;
    lhs->internal = aux.internal;
    aux.internal = swap;
    
    bignum_destroy(&aux);
    return SUCCESS;
}

result_code bignum_divide(bignum* lhs, const bignum* rhs, bignum* remainder) {
    if (rhs->internal->data == 0 && rhs->internal->next == NULL) {
        return FAIL_DIVIDE_BY_ZERO;
    }

    if (bignum_cmp(rhs, lhs) > 0) {
        // Se rhs > lhs, basta zerar o lado esquerdo.
        bignum_destroy(lhs);
        bignum_init(lhs);
        return SUCCESS;
    }

    result_code result = SUCCESS;

    bignum quotient = {0};
    if ((result = bignum_init(&quotient)) != SUCCESS)
        goto finish;

    bool returns_remainder = remainder != NULL;

    if (!returns_remainder) {
        remainder = (bignum*) alloca(sizeof(bignum));
        if ((result = bignum_init(remainder)) != SUCCESS)
            goto finish;
    }

    reverse(lhs);

    for (node_ptr it = lhs->internal; it != NULL; it = it->next) {
        if (remainder->internal->data != 0 || remainder->internal->next != NULL) {
            node_ptr lsb = (node_ptr) malloc(sizeof(struct _bignum_data));
            if (lsb == NULL) {
                result = FAIL_OOM;
                goto finish;
            }

            lsb->data = it->data;
            lsb->next = remainder->internal;
            remainder->internal = lsb;
        } else {
            remainder->internal->data = it->data;
        }

        if (bignum_cmp(remainder, rhs) >= 0) {
            bignum_item q = divide_base(remainder, rhs);

            if (quotient.internal->data != 0 || quotient.internal->next != NULL) {
                node_ptr lsb = (node_ptr) malloc(sizeof(struct _bignum_data));
                if (lsb == NULL) {
                    result = FAIL_OOM;
                    goto finish;
                }

                lsb->data = q;
                lsb->next = quotient.internal;
                quotient.internal = lsb;
            } else {
                quotient.internal->data = q;
            }
        }
    }

    node_ptr swap = lhs->internal;
    lhs->internal = quotient.internal;
    quotient.internal = swap;
    
finish:
    if (quotient.internal) bignum_destroy(&quotient);
    if (!returns_remainder) {
        if (remainder->internal) bignum_destroy(remainder);
    }
    return result;
}
