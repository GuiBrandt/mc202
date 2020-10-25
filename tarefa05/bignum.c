/**
 * @file bignum.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação do ADT de número bem grande.
 * 
 * A implementação usa uma lista ligada onde cada elemento da lista é um
 * número por si só. Por questão de eficiência, os itens são números inteiros
 * relativamente grandes.
 * 
 * Na prática, isso funciona como um número em base 10^9 onde cada "dígito"
 * é um elemento na lista. A natureza posicional do sistema de representação
 * numérica ainda se aplica, e nós mais próximos da cabeça são menos
 * significativos.
 * 
 * Por exemplo, a lista 0 -> 1 tem valor 0*1 + 1*10^9 = 10^9.
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

#include <stdlib.h>     // malloc, free, etc.

#include <stdbool.h>    // bool
#include <string.h>     // strlen

#include <assert.h>     // assert

//============================================================================
// Definições específicas da implementação
//============================================================================

/**
 * @brief Valor máximo para um único item na lista, i.e., a base numérica
 *        adotada para representação dos números. 
 * 
 * Este valor não pode ser maior que 1000000000 porque é necessário que o
 * produto de quaisquer dois itens ocupe no máximo 64-bits para o processo de
 * multiplicação.
 */
#define ITEM_MAX 1000000000U

/**
 * @brief Tipo numérico para um item no número grandão.
 * 
 * O tipo numérico deve ser grande o suficiente para armazenar o produto de
 * dois inteiros na base numérica escolhida
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
 * @brief Aloca e inicializa um nó com 0. O(1).
 * 
 * @return ponteiro para o nó alocado ou NULL em caso de falha.
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
// Utilitários (Tratamento de erro)
//============================================================================

#define TRY(expr, on_fail) do {     \
    result_code __result = (expr);  \
    on_fail;                        \
} while (0)

#define ON_FAIL(expr) do {          \
    if (__result != SUCCESS) {      \
        expr;                       \
        return __result;            \
    }                               \
} while (0)

#define EXPECT(expr) TRY(expr, ON_FAIL())

//============================================================================
// Utilitários (Lista ligada)
//============================================================================

/**
 * @brief Adiciona um próximo para um nó caso ele não tenha. O(1).
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
 * @brief Libera todos os nós a partir de um nó dado (inclusive).
 * 
 * @param node Nó a partir do qual liberar.
 */
void drop_from(node_ptr node) {
    for (node_ptr it = node; it != NULL;) {
        node_ptr aux = it;
        it = it->next;
        free(aux);
    }
}

/**
 * @brief Verifica se um número grande a partir de um nó equivale a 0. O(1).
 * 
 * @param node Nó inicial do número grande.
 * @return true se o número for 0 e false caso contrário. 
 */
bool is_zero(const node_ptr node) {
    return node->next == NULL && node->data == 0;
}

/**
 * @brief Reverte a lista ligada de um número grande in-place. O(n).
 * 
 * @param ptr Ponteiro para o número grande.
 */
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

/**
 * @brief Adiciona um valor ao início da lista em um número grande. O(1).
 * 
 * Equivalente a deslocar para a esquerda e definir o bit menos significativo,
 * mas em base ITEM_MAX.
 * 
 * @param dest Número grande sendo manipulado.
 * @param data Valor número do item menos significativo.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code push_front(bignum* dest, bignum_item data) {
    if (is_zero(dest->internal)) {
        dest->internal->data = data;
        
    } else {
        node_ptr lsb = (node_ptr) malloc(sizeof(struct _bignum_data));
        if (lsb == NULL)
            return FAIL_OOM;

        lsb->data = data;
        lsb->next = dest->internal;
        dest->internal = lsb;
    }

    return SUCCESS;
}

#ifdef NDEBUG
#define assert_valid ((void)0)
#else
/**
 * @brief Verifica que um ponteiro de número grande é válido. 
 */
void assert_valid(const bignum* ptr) {
    assert(ptr != NULL);
    assert(ptr->internal != NULL);
}
#endif

//============================================================================
// Utilitários (Operações)
//============================================================================

/**
 * @brief Adiciona um valor inteiro a um número grande a partir de um nó,
 *        aplicando carry over.
 * 
 * O(n), onde n = número de dígitos correspondentes à lista começando no nó
 *                dado.
 * 
 * Essa função aloca nós conforme necessário, caso o carry over aconteça.
 * 
 * @param node Nó inicial para aplicar a soma.
 * @param n Valor a somar.
 * @return SUCCESS ou FAIL_OOM.
 */
result_code add_with_carry(node_ptr node, bignum_item n)
    __attribute__((warn_unused_result));

result_code add_with_carry(node_ptr node, bignum_item n) {

    // Esta soma sempre é segura (i.e. não estoura o tipo inteiro), desde que N
    // obedeça os limites de tamanho.
    node->data += n;

    // Por outro lado, temos que considerar que podemos estourar o limite
    // e temos que "carregar" o que sobrou para os próximos nós.
    for (
        node_ptr current_carry = node;
        current_carry->data >= ITEM_MAX;
        current_carry = current_carry->next
    ) {
        int carry = current_carry->data / ITEM_MAX;
        current_carry->data %= ITEM_MAX;

        EXPECT(extend_if_needed(current_carry));
        current_carry->next->data += carry;
    }

    return SUCCESS;
}

/**
 * @brief Soma um número grande a partir de um nó em outro.
 * 
 * O(n.m), onde n = dígitos em rhs e m = dígitos no número representado pela
 *                  lista começando no nó de destino.
 * 
 * @param node Nó de destino.
 * @param rhs Número grande a ser somado.
 * 
 * @return SUCESS ou FAIL_OOM.
 */
result_code add_at_node(node_ptr node, const bignum* rhs)
    __attribute__((warn_unused_result));

result_code add_at_node(node_ptr node, const bignum* rhs) {

    // Percorre o número grande sendo somado e a lista começando no nó dado e
    // soma os valores a cada passo, aplicando carry over.
    // O(m) * T_loop
    for (
        node_ptr current_rhs = rhs->internal;
        current_rhs != NULL;
        node = node->next, current_rhs = current_rhs->next
    ) {
        bignum_item n = current_rhs->data;

        EXPECT(add_with_carry(node, n)); // O(n)

        if (current_rhs->next != NULL)
            EXPECT(extend_if_needed(node));
    } // T_loop = O(n)

    return SUCCESS;
}

/**
 * @brief Carrega o "empréstimo" da subtração até encontrar algum nó não-nulo.
 * 
 * O(n), n = dígitos no número correspondente ao nó.
 * 
 * @param current Nó subtraído.
 */
void carry_subtract(node_ptr current) {
    node_ptr current_carry = current->next,
             prev = current;

    // O(n)
    while (current_carry->data == 0) {
        current_carry->data = ITEM_MAX - 1;
        prev = current_carry;
        current_carry = current_carry->next;
    }

    current_carry->data--;

    // Se formos deixar algum zero à esquerda, liberamos o nó
    // correspondente.
    if (is_zero(current_carry)) {
        prev->next = NULL;
        free(current_carry);
    }
}

/**
 * @brief Remove zeros à esquerda (no caso, à direita na lista).
 * 
 * @param root Nó raíz a partir de onde buscar zeros.
 */
void drop_trailing_zeroes(node_ptr root) {
    node_ptr last_nonzero = root;
    for (node_ptr it = root; it != NULL; it = it->next) {
        if (it->data != 0) last_nonzero = it;
    }

    drop_from(last_nonzero->next);

    last_nonzero->next = NULL;
}

/**
 * @brief Aplica subtração assumindo que lhs >= rhs.
 * 
 * O(n.m), onde n = dígitos em lhs e m = dígitos em rhs.
 * 
 * @param lhs Minuendo (recebe o resultado).
 * @param rhs Subtraendo.
 */
void subtract_base(bignum* lhs, const bignum* rhs) {
    assert(bignum_cmp(lhs, rhs) >= 0);

    // O(m) * T_loop
    for (
        node_ptr current_lhs = lhs->internal,
                 prev_lhs = NULL,
                 current_rhs = rhs->internal;
        
        current_rhs != NULL;

        current_lhs = (prev_lhs = current_lhs)->next,
        current_rhs = current_rhs->next
    ) {
        assert(current_lhs != NULL);

        if (current_lhs->data >= current_rhs->data) { // O(1)
            current_lhs->data -= current_rhs->data;
        } else {
            current_lhs->data += ITEM_MAX;
            current_lhs->data -= current_rhs->data;
            
            carry_subtract(current_lhs); // O(n)
        }
    } // T_loop = O(n)

    drop_trailing_zeroes(lhs->internal);
}

/**
 * @brief Copia o produto de um número grande por um número não tão grande para
 *        um número grande.
 * 
 * O(n.m), n = número de dígitos no número original, m = número de dígitos no
 *             número de destino no início da função.
 * 
 * @param dest Número grande de destino.
 * @param source Número grande original.
 * @param mult Multiplicador não tão grande.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code multiply_partial(
    node_ptr dest,
    const bignum* source,
    bignum_item mult
) {
    if (mult == 0)
        return SUCCESS;

    // O(n) * T_loop
    for (
        node_ptr current_source = source->internal,
                 current_dest = dest;
        
        current_source != NULL;
        
        current_source = current_source->next, 
        current_dest = current_dest->next
    ) {
        bignum_item p = current_source->data * mult;

        EXPECT(add_with_carry(current_dest, p)); // O(m)

        if (current_source->next != NULL) {
            EXPECT(extend_if_needed(current_dest));
        }
    } // T_loop = O(n)

    return SUCCESS;
}

/**
 * @brief Caso simplificado da divisão, onde lhs / rhs é garantido ter
 *        exatamente um dígito na base usada (i.e., é < ITEM_MAX).
 * 
 * Faz busca binária nos valores possíveis para determinar o quociente,
 * retornando o maior valor menor ou igual ao quociente.
 * 
 * O(n.m), onde n = dígitos em lhs e m = dígitos em rhs.
 * 
 * Ao fim da função, o dividendo fica com o resto da divisão.
 * 
 * @param lhs Dividendo.
 * @param rhs Divisor.
 * @param out Quociente.
 * 
 * @return SUCCESS ou FAIL_OOM.
 */
result_code divide_base(bignum* lhs, const bignum* rhs, bignum_item* out) {
    bignum product;
    
    bignum_item left = 0, right = ITEM_MAX - 1, best = 0;
    while (left <= right) { // O(log ITEM_MAX = O(1)) * T_loop
        bignum_item mid = left + (right - left) / 2;

        EXPECT(bignum_init(&product));

        // O(n), product = 0 e tem sempre O(1) dígito
        TRY(multiply_partial(product.internal, rhs, mid),
            ON_FAIL(bignum_destroy(&product)));

        int cmp = bignum_cmp(&product, lhs);
        if (cmp > 0) {
            right = mid - 1;

        } else if (cmp < 0) {
            left = mid + 1;
            best = mid;

        } else {
            best = mid;
            left = right + 1;
        }

        bignum_destroy(&product);
    } // T_loop = O(n)

    EXPECT(bignum_init(&product));
    
    TRY(multiply_partial(product.internal, rhs, best),
            ON_FAIL(bignum_destroy(&product)));

    TRY(bignum_subtract(lhs, &product), 
        ON_FAIL(bignum_destroy(&product)));

    bignum_destroy(&product);

    *out = best; // Maior item menor que o desejado

    return SUCCESS;
}

//============================================================================
// Implementações (Contrato)
//============================================================================

// O(1)
result_code bignum_init(bignum* dest) {
    assert(dest != NULL);

    dest->internal = new_node();

    if (dest->internal == NULL)
        return FAIL_OOM;
    
    return SUCCESS;
}

// O(n)
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

// O(n)
void bignum_destroy(bignum* dest) {
    assert_valid(dest);
    drop_from(dest->internal);
}

// O(n), n = tamanho da string
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
    for (int i = strlen(str) - 1, pow = 1; i >= 0; i--, pow *= 10) {
        if (pow >= ITEM_MAX) {
            // Aqui assumimos que a base é sempre uma potência de 10, então
            // não é necessário fazer carry over dos valores nem nada do
            // gênero
            current = current->next = new_node();
            if (current == NULL)
                return FAIL_OOM;

            pow = 1;
        }

        assert('0' <= str[i] && str[i] <= '9');

        int digit = str[i] - '0';
        current->data += digit * pow;
    }

    return SUCCESS;
}

// O(n + m), n = dígitos no número e m = tamanho da string
result_code bignum_sprintf(char* dest, size_t len, const bignum* source) {
    assert(dest != NULL);
    assert_valid(source);

    node_ptr current = source->internal;
    
    size_t i;
    int pow = 1;
    for (i = 0; current != NULL && i < len - 1; i++) {
        bignum_item q = current->data / pow;
        if (q == 0 && current->next == NULL)
            break;

        int digit = q % 10;

        dest[i] = '0' + digit;

        pow *= 10;
        if (pow >= ITEM_MAX) {
            current = current->next;
            pow = 1;
        }
    }

    if (i == len)
        return FAIL_STRING_OVERFLOW;

    // Temos que inverter a string, porque na lista os valores menos
    // significativos ficam próximos do começo e em uma string geralmente
    // queremos o contrário.
    for (size_t j = 0; j < i / 2; j++) {
        char swap = dest[j];
        dest[j] = dest[i - j - 1];
        dest[i - j - 1] = swap;
    }

    // Caso especial: n = 0.
    if (i == 0) {
        dest[i] = '0';
        i++;
    }

    dest[i] = '\0';

    return SUCCESS;
}

// O(n + m)
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
    // esse caso. Aqui é importante que zeros à esquerda não existam.
    if (current_lhs == NULL && current_rhs != NULL) {
        return -1;
    } else if (current_lhs != NULL && current_rhs == NULL) {
        return 1;
    }

    // Se as listas tinham o mesmo tamanho, a comparação dos últimos itens
    // diferentes nelas dão o resultado.
    return current_cmp;
}

// O(n.m)
result_code bignum_add(bignum* lhs, const bignum* rhs) {
    assert_valid(lhs);
    assert_valid(rhs);

    node_ptr current_lhs = lhs->internal;
    return add_at_node(current_lhs, rhs);
}

// O(n.m)
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
        EXPECT(bignum_init(&aux));
        TRY(bignum_copy(&aux, rhs), ON_FAIL(bignum_destroy(&aux))); // O(m)
        
        node_ptr swap = lhs->internal;
        lhs->internal = aux.internal;
        aux.internal = swap;

        subtract_base(lhs, &aux); // O(n.m)
        bignum_destroy(&aux);
        return SUCCESS;

    } else {
        subtract_base(lhs, rhs); // O(n.m)
        return SUCCESS;
    }
}

// O(n.m)
result_code bignum_multiply(bignum* lhs, const bignum* rhs) {
    assert_valid(lhs);
    assert_valid(rhs);

    // Cria um bignum auxiliar para receber os resultados parciais sem destruir
    // o lado esquerdo, que é reutilizados durante o processo.
    bignum aux;
    EXPECT(bignum_init(&aux));

    // Caso especial: se lhs = 0, o retorno é 0.
    // Este caso é tratado separadamente porque é o único caso em que seria
    // possível introduzir zeros à esquerda no número, por conta do próximo
    // passo, e zeros à esquerda atrapalham no resto do programa.
    if (lhs->internal->next == NULL && is_zero(lhs->internal)) {
        bignum_destroy(&aux);
        return SUCCESS;
    }

    // Percorremos o lado direito para multiplicar pelos elementos do lado
    // esquerdo, subindo também as casas decimais do resultado.
    // O(m) * T_loop
    for (
        node_ptr current_rhs = rhs->internal,
                 current_aux = aux.internal;

        current_rhs != NULL;
        
        current_rhs = current_rhs->next,
        current_aux = current_aux->next
    ) {
        // O(n), porque current_aux tem sempre tamanho 1 a cada iteração
        TRY(multiply_partial(current_aux, lhs, current_rhs->data),
            ON_FAIL(bignum_destroy(&aux)));

        if (current_rhs->next != NULL) {
            TRY(extend_if_needed(current_aux), ON_FAIL(bignum_destroy(&aux)));
        }
    } // T_loop = O(n.m)

    // Ao final, trocamos o auxiliar e o lhs
    node_ptr swap = lhs->internal;
    lhs->internal = aux.internal;
    aux.internal = swap;
    
    bignum_destroy(&aux);
    return SUCCESS;
}

result_code bignum_divide(bignum* lhs, const bignum* rhs) {
    if (rhs->internal->data == 0 && rhs->internal->next == NULL) {
        return FAIL_DIVIDE_BY_ZERO;
    }

    if (bignum_cmp(rhs, lhs) > 0) {
        // Se rhs > lhs, basta zerar o lado esquerdo.
        bignum_destroy(lhs);
        return bignum_init(lhs);
    }

    bignum quotient;
    EXPECT(bignum_init(&quotient));

    bignum remainder;
    TRY(bignum_init(&remainder), ON_FAIL(bignum_destroy(&quotient)));

    // Invertemos o dividendo para aplicar o algoritmo de divisão.
    // Daria pra usar uma lista duplamente ligada, mas como só precisamos
    // disso nessa função não compensa o esforço e custo de manutenção.
    reverse(lhs);

    #define CLEANUP()               \
        bignum_destroy(&quotient);  \
        bignum_destroy(&remainder);

    for (node_ptr it = lhs->internal; it != NULL; it = it->next) {
        TRY(push_front(&remainder, it->data), ON_FAIL(CLEANUP()));

        bignum_item q;
        if (bignum_cmp(&remainder, rhs) >= 0) {
            TRY(divide_base(&remainder, rhs, &q), ON_FAIL(CLEANUP()));
        } else {
            q = 0;
        }

        TRY(push_front(&quotient, q), ON_FAIL(CLEANUP()));
    }

    node_ptr swap = lhs->internal;
    lhs->internal = quotient.internal;
    quotient.internal = swap;
    
    CLEANUP();
    
    return SUCCESS;
}
