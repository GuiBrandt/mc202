/**
 * @file priority_queue.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação de fila de prioridade com remoção com Beap [MS80].
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa09.html
 * 
 * Todas as operações têm complexidade O(√n), e assim como um Heap, o Beap
 * (ou Heap biparental) é representado implicitamente em um array, ocupando
 * O(n) espaço [MS80].
 * 
 * Para os valores de n possíveis na tarefa¹, o tempo proporcional de inserção
 * e remoção de máximo proporcional a 2√2n do Beap não passa muito de cinco
 * vezes log n, e isso é compensado durante os cancelamentos, onde o Beap é
 * cerca de cinco vezes (provavelmente mais) mais rápido³.
 * 
 * ¹ As restrições especificam < 500 operações, sendo que uma certamente é um T
 *   e não adiciona nenhum cliente na fila. Sendo assim, restam 498 operações,
 *   das quais metade deve inserir um cliente e metade deve removê-lo da fila².
 *   Logo, no máximo, a fila tem 249 clientes.
 * 
 * ² A tarefa não especifica essa relação, mas parece uma premissa plausível
 *   assumir que a fila termina vazia.
 * 
 * ³ Para n = 249, os valores são:
 *      - Inserção/Remover máximo: 2√498 / lg 249 ≈ 5,6070
 *      - Deletar arbitário: (249 + lg 249) / 2√498 ≈ 5.7573
 *   Vale também observar que durante a busca linear no Heap, as comparações
 *   das chaves tomam um tempo considerável. Isso é extremamente reduzido
 *   nesta implementação. Por isso, o fator de melhoria no cancelamento é,
 *   provavelmente, ainda maior.
 * 
 * [MS80] MUNRO, J. Ian; SUWANDA, Hendra.
 *        Implicit data structures for fast search and update.
 *        Journal of Computer and System Sciences, v. 21, n. 2, p. 236-250,
 *        1980. https://doi.org/10.1016/0022-0000(80)90037-9
 */

#include "priority_queue.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <assert.h>
#include <math.h>

#include "map.h"
#include "xmalloc.h"

//=============================================================================
// Beap
//=============================================================================

struct priority_queue {
    const customer* customers[249];
    map* ratings;
    size_t size;
    size_t height;
};

/**
 * @brief Calcula o índice no array do primeiro elemento com dada profundidade
 *        no Beap.
 * 
 * @param depth profundidade desejada.
 * 
 * @return índice no array,
 */
int first_index(int depth) {
    return depth * (depth - 1) / 2;
}

/**
 * @brief Calcula o índice no array do último elemento com dada profundidade no
 *        Beap.
 * 
 * @param depth profundidade desejada.
 * 
 * @return índice no array,
 */
int last_index(int depth) {
    return first_index(depth + 1) - 1;
}

/**
 * @brief Calcula o índice do pai esquerdo de um elemento com dada profundidade
 *        no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do pai esquerdo.
 */
int left_parent(int i, int depth) {
    return i - depth;
}

/**
 * @brief Calcula o índice do pai direito de um elemento com dada profundidade
 *        no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do pai direito.
 */
int right_parent(int i, int depth) {
    return i - depth + 1;
}

/**
 * @brief Calcula o índice do filho esquerdo de um elemento com dada
 *        profundidade no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do filho esquerdo.
 */
int left_child(int i, int depth) {
    return i + depth;
}

/**
 * @brief Calcula o índice do filho direito de um elemento com dada
 *        profundidade no Beap.
 * 
 * @param i índice do elemento.
 * @param depth profundidade.
 * 
 * @return índice do filho direito.
 */
int right_child(int i, int depth) {
    return i + depth + 1;
}

/**
 * @brief Sobe um elemento no Beap até que esteja na posição apropriada.
 * 
 * Complexidade de tempo: O(√n) [MS80].
 * 
 * @param q o Beap.
 * @param c o cliente sendo movido.
 * @param i o índice do elemento no Beap.
 * @param depth a profundidade do elemento no Beap.
 */
void sift_up(priority_queue* q, const customer* c, int i, int depth) {
    for (; depth > 1; depth--) {

        // Calculamos o índice e prioridade dos pais do elemento, usando
        // INFINITY caso algum dois pais seja inválido. Isso tem o efeito de
        // ignorar aquele pai.
        int l_i = -1, r_i = -1;
        double l_rating, r_rating;
        
        if (i != first_index(depth)) {
            l_i = left_parent(i, depth);
            l_rating = q->customers[l_i]->rating;

        } else { // O primeiro elemento do nível não tem pai esquerdo
            l_rating = INFINITY;
        }

        if (i != last_index(depth)) {
            r_i = right_parent(i, depth);
            r_rating = q->customers[r_i]->rating;

        } else { // O último elemento do nível não tem pai direito
            r_rating = INFINITY;
        }

        // Se o elemento é maior qualquer um dos pais, trocamos ele pelo menor
        // dos pais
        if (c->rating > l_rating || c->rating > r_rating) {
            if (l_rating < r_rating) {
                assert(l_i != -1);

                q->customers[i] = q->customers[l_i];
                i = l_i;
            } else {
                assert(r_i != -1);

                q->customers[i] = q->customers[r_i];
                i = r_i;
            }

        // Se não, encontramos o lugar correto para o elemento
        } else {
            q->customers[i] = c;
            return;
        }
    }

    // Caso especial: a função não se comporta muito bem para depth = 1 (em
    // específico por conta da definição de right_parent), então tratamos o
    // caso onde o elemento é o maior separadamente.
    if (c->rating > q->customers[0]->rating) {
        q->customers[i] = q->customers[0];
        q->customers[0] = c;
    } else {
        q->customers[i] = c;
    }
}

/**
 * @brief Desce um elemento no Beap até que esteja na posição apropriada.
 * 
 * Complexidade de tempo: O(√n) [MS80].
 * 
 * Simétrica a sift_up.
 * 
 * @param q o Beap.
 * @param c o cliente sendo movido.
 * @param i o índice do elemento no Beap.
 * @param depth a profundidade do elemento no Beap.
 */
void sift_down(priority_queue* q, const customer* c, int i, int depth) {
    for (; depth <= q->height; depth++) {
        int l_i = left_child(i, depth), r_i = -1;
        double l_rating, r_rating;
        
        if (l_i < q->size) {
            l_rating = q->customers[l_i]->rating;

            r_i = right_child(i, depth);

            if (r_i < q->size) {
                r_rating = q->customers[r_i]->rating;
            } else {
                r_rating = -INFINITY;
            }
        } else {
            l_rating = r_rating = -INFINITY;
        }

        if (c->rating < l_rating || c->rating < r_rating) {
            if (l_rating > r_rating) {
                assert(l_i != -1);

                q->customers[i] = q->customers[l_i];
                i = l_i;
            } else {
                assert(r_i != -1);

                q->customers[i] = q->customers[r_i];
                i = r_i;
            }
        } else {
            q->customers[i] = c;
            return;
        }
    }

    q->customers[i] = c;
}

/**
 * @brief Remove um elemento do Beap dado seu índice e profundidade.
 * 
 * Complexidade de tempo: O(√n) [MS80].
 * 
 * @param q o Beap.
 * @param i o índice do elemento no Beap.
 * @param depth a profundidade do elemento no Beap.
 */
void delete_at(priority_queue* q, int i, int depth) {
    q->size--;

    const customer* c = q->customers[q->size];

    // No máximo um dos dois tem efeito, mas não tem vantagem verificar antes
    // qual deve acontecer (a lógica seria a mesma da função).
    sift_up(q, c, i, depth);
    sift_down(q, c, i, depth);
    
    if (q->size < first_index(q->height)) {
        q->height--;
    }
}

/**
 * @brief Encontra o índice e profundidade do elemento com prioridade dada no
 *        Beap.
 * 
 * Complexidade de tempo: O(√n) [MS80].
 * 
 * @param q o Beap.
 * @param rating a prioridade do elemento.
 * @param depth ponteiro de saída para a profundidade do elemento. 
 * 
 * @return índice do elemento no Beap.
 */
int find(priority_queue* q, double rating, int* depth) {

    // Começamos pelo índice mais à direita no Beap, este pode ser o último
    // item do penúltimo nível ou o último item do último nível, se (e somente
    // se) estiver cheio.
    int i, d;
    
    if (q->size - 1 == last_index(q->height)) {
        i = q->size - 1;
        d = q->height;
    } else {
        i = last_index(q->height - 1);
        d = q->height - 1;
    }

    // A partir disso, percorremos o Beap como uma matriz triangular onde
    // linhas e colunas estão ordenadas, buscando pelo elemento desejado.
    while (i < q->size && q->customers[i]->rating != rating) {

        // O pai esquerdo está diretamente à esquerda na matriz.
        if (rating > q->customers[i]->rating) {
            i = left_parent(i, d);
            d--;

        } else {
            int c_i = left_child(i, d);
            
            // Se estivermos na diagonal, andamos na diagonal. Devido à
            // representação do beap como array, para isso basta diminuir o
            // índice (a profundidade também não muda).
            if (c_i >= q->size || (d != 1 && i == last_index(d))) {
                i--;

            // Se não, vamos para o filho esquerdo, que está diretamente abaixo
            // na matriz.
            } else {
                i = c_i;
                d++;
            }
        }
    }

    assert(i < q->size);

    *depth = d;
    return i;
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

priority_queue* make_queue() {
    priority_queue* q = (priority_queue*) xmalloc(sizeof(priority_queue));

    q->size = 0;
    q->height = 0;
    q->ratings = make_map();

    return q;
}

/**
 * Complexidade de tempo: O(√n) [MS80].
 * 
 * A intuição é que o Beap forma uma matriz triangular, cuja altura é sempre
 * proporcional a √(2n). Como o "beapify" toma tempo constante por nível
 * (algumas comparações e no máximo uma troca), segue a complexidade.
 */
void enqueue(priority_queue* q, const customer* c) {
    map_add(q->ratings, c->name, c->rating);

    int n = q->size;
    q->size++;

    if (q->size == 1) {
        q->customers[n] = c;
        q->height = 1;
        return;
    }

    sift_up(q, c, q->size - 1, q->height);

    if (q->size > last_index(q->height)) {
        q->height++;
    }
}

/**
 * Complexidade de tempo: O(√n) [MS80]. Idem enqueue.
 */
customer* dequeue(priority_queue* q) {
    if (q->size == 0) {
        return NULL;
    }

    customer* c = (customer*) q->customers[0];
    delete_at(q, 0, 1);

    return c;
}

/**
 * Complexidade de tempo: O(√n) [MS80].
 * 
 * Assim como as duas outras operações, só precisamos fazer uma troca (tempo
 * constante) e "beapify", que toma tempo O(√n).
 * 
 * Isso só se aplica se tivermos o índice do elemento em tempo O(√n), no
 * entanto.
 * 
 * Para isso, usa-se um dicionário com tempo de leitura constante¹. Dessa
 * forma, podemos buscar a prioridade para um nome dado e usá-la para buscar
 * pelo elemento no Beap em tempo O(√n), mantendo a complexidade da operação
 * como um todo.
 * 
 * ¹ Mais especificamente, com tempo O(k), onde k é o tamanho do nome;
 *   no caso da tarefa, k <= 16, então é seguro dizer que o tempo é de fato
 *   constante (e relativamente pequeno).
 */
customer* cancel(priority_queue* q, const char* name) {
    double rating = map_get(q->ratings, name);

    int depth;
    int index = find(q, rating, &depth);

    assert(index != -1);

    customer* c = (customer*) q->customers[index];

    delete_at(q, index, depth);

    return c;
}

void destroy_queue(priority_queue* q) {
    destroy_map(q->ratings);
    free(q);
}
