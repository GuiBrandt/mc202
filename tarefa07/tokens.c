/**
 * @file tokens.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definições do ADT para os tokens do mensageiro.
 */

#include "tokens.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

//=============================================================================
// Definições
//=============================================================================

/**
 * @brief Enumeração de tipos de cartão.
 * 
 * A ideia é que temos dois tipos de cartão:
 * - Cartões SIMPLES, com um texto de até 5 caracteres no verso.
 * - Cartões COMPOSTOS, que são sempre resultado de uma operação de troca de
 *   tríade. Ao invés de um texto, esses cartões têm uma ABB de cartões.
 */
typedef enum {
    TOKEN_SIMPLES,
    TOKEN_COMPOSTO
} tipo_token;

/**
 * @brief Estrutura de cartão numerado.
 */
typedef struct {
    int num;
    tipo_token tipo;
    union {
        char msg[TOKEN_MSG_MAXLEN + 1]; // se tipo = TOKEN_SIMPLES
        saco componentes;               // se tipo = TOKEN_COMPOSTO
    };
} token;

/**
 * @brief Estrutura usada para a representação interna do saco de cartões.
 * 
 * É um nó de árvore binária com um cartão.
 */
typedef struct saco_internal {
    token val;
    struct saco_internal* esq;
    struct saco_internal* dir;
} node;

//=============================================================================
// Implementação (Auxiliares)
//=============================================================================

/**
 * @brief Constrói um nó folha de árvore binária para um cartão.
 * 
 * Termina o programa caso falhe em alocar memória.
 * 
 * @param t o cartão.
 * @return um nó folha com o cartão dado.
 */
node* novo_no(token t) {
    node* n = (node*) malloc(sizeof(node));

    if (n == NULL) {
        fprintf(stderr, "Erro fatal: Out of memory\n");
        exit(137);
    }

    n->val = t;
    n->esq = n->dir = NULL;
    return n;
}

/**
 * @brief Busca pelo nó pai para um certo valor numérico de cartão numa ABB.
 * 
 * Esta função não depende do valor existir na árvore, e pode ser usada para
 * inserção. Se o valor não existe na árvore, o nó retornado é garantido não
 * ter um filho na direção do valor.
 * 
 * A função retorna NULL sse o nó raíz tem o valor buscado.
 * 
 * @param raiz o nó raiz da árvore.
 * @param num o valor numérico buscado.
 * 
 * @return o nó pai para o valor dado.
 * 
 * @note Essa função é separada da função de busca para evitar duplicação de
 *       código. O ponteiro do pai é necessário para a remoção, e adicioná-lo
 *       ao nó em si adicionaria complexidade.
 */
node* busca_pai(node* raiz, int num) {
    node* anterior = NULL, *atual = raiz;

    while (atual != NULL) {
        int valor = atual->val.num;

        if (num == valor) {
            break;
        }

        anterior = atual;

        if (num < valor) {
            atual = atual->esq;
        } else { // num > valor
            atual = atual->dir;
        }
    }

    return anterior;
}

/**
 * @brief Busca pelo nó contendo um cartão com o valor numérico dado numa ABB.
 * 
 * Esta função retorna o nó pai caso o valor passado não exista na árvore.
 * 
 * @param raiz o nó raíz da árvore.
 * @param num o valor numérico buscado.
 * 
 * @return o nó contendo o cartão buscado.
 */
node* busca(node* raiz, int num) {
    node* anterior = busca_pai(raiz, num);

    if (anterior == NULL) {
        return (node*) raiz;
    }

    if (anterior->esq && num == anterior->esq->val.num) {
        return anterior->esq;
    }

    if (anterior->dir && num == anterior->dir->val.num) {
        return anterior->dir;
    }
        
    return anterior;
}

/**
 * @brief Insere um nó numa ABB.
 * 
 * Esta função assume que valores repetidos não serão inseridos.
 * 
 * @param raiz o nó raíz da árvore.
 * @param no o nó sendo inserido.
 * 
 * @return a nova raíz da árvore.
 */
node* insere(node* raiz, node* no) {
    if (raiz == NULL) {
        return no;
    }

    int valor = no->val.num;
    node* anterior = busca_pai(raiz, valor);

    if (valor < anterior->val.num) {
        assert(anterior->esq == NULL);
        anterior->esq = no;
    } else {
        assert(anterior->dir == NULL);
        anterior->dir = no;
    }

    return raiz;
}

/**
 * @brief Remove e retorna o menor nó de uma árvore.
 * 
 * @param raiz o nó raíz da árvore.
 * 
 * @return o nó removido.
 */
node* pop_min(node* raiz) {
    if (raiz->esq == NULL) {
        return raiz;
    }
    
    if (raiz->esq->esq == NULL) {
        node* rem = raiz->esq;
        raiz->esq = rem->dir;
        return rem;
    }

    return pop_min(raiz->esq); // Recursão de cauda
}

/**
 * @brief Remove e retorna o maior nó de uma árvore.
 * 
 * @param raiz o nó raíz da árvore.
 * 
 * @return o nó removido.
 */
node* pop_max(node* raiz) {
    if (raiz->dir == NULL) {
        return raiz;
    }
    
    if (raiz->dir->dir == NULL) {
        node* rem = raiz->dir;
        raiz->dir = rem->esq;
        return rem;
    }

    return pop_max(raiz->dir); // Recursão de cauda
}

/**
 * @brief Substitui um nó pelo maior filho esquerdo ou menor filho direito.
 * 
 * A função retorna NULL sse o nó passado é um nó folha.
 * 
 * @param raiz o nó a ser substituído.
 * 
 * @return o nó que tomou o lugar do nó dado.
 */
node* substitui(node* raiz) {
    if (!(raiz->dir || raiz->esq)) {
        free(raiz);
        return NULL;
    }

    node* novo;
    
    if (raiz->dir) {
        novo = pop_min(raiz->dir);
    } else {
        novo = pop_max(raiz->esq);
    }

    if (novo != raiz->dir) {
        novo->dir = raiz->dir;
    }

    if (novo != raiz->esq) {
        novo->esq = raiz->esq;
    }

    free(raiz);

    return novo;
}

/**
 * @brief Remove um nó de uma ABB dado seu valor numérico.
 * 
 * Assume que o valor passado existe na árvore.
 * 
 * @param raiz o nó raíz da árvore.
 * @param num o valor numérico.
 * 
 * @return a nova raíz da árvore.
 */
node* remove_valor(node* raiz, int num) {
    node* pai = busca_pai(raiz, num);

    if (pai == NULL) {
        return substitui(raiz);
    }

    if (pai->esq && num == pai->esq->val.num) {
        pai->esq = substitui(pai->esq);
    } else {
        pai->dir = substitui(pai->dir);
    }

    return raiz;
}

/**
 * @brief Adiciona um token ao saco de cartões.
 * 
 * @param s o saco de cartões.
 * @param t o token.
 */
void add_token_impl(saco* s, token t) {
    node* novo = novo_no(t);
    s->internal = insere(s->internal, novo);    
}

/**
 * @brief Busca um par de nós no saco de cartões com uma soma dada.
 * 
 * Percorre a árvore em pré-ordem e busca pelo complemento da soma para cada
 * valor. O tempo dessa busca é O(n log n). Acho que seria possível fazer em
 * O(n) com ponteiro para o pai (ou usando um hashset!), mas não houve problema
 * de tempo na tarefa, então preferi deixar assim, para evitar adicionar
 * complexidade.
 * 
 * Quando a função encontra um par, retorna 1 e escreve os ponteiros para os
 * nós encontrados nos ponteiros `a` e `b`, passados por parâmetro.
 * 
 * @param s o saco de cartões.
 * @param raiz o nó atual do percurso em pré-ordem.
 * @param soma a soma sendo buscada.
 * @param a ponteiro de saída para um nó.
 * @param b ponteiro de saída para o outro nó.
 * @return 1 caso encontre um par, 0 caso contrário.
 */
int busca_par(saco* s, node* raiz, int soma, node** a, node** b) {
    if (raiz == NULL) {
        return 0;
    }

    int subsoma = soma - raiz->val.num;

    node* no = busca(s->internal, subsoma);
    if (no->val.num == subsoma) {
        *a = raiz;
        *b = no;
        return 1;
    }

    return busca_par(s, raiz->esq, soma, a, b)
        || busca_par(s, raiz->dir, soma, a, b);
}

/**
 * @brief Busca uma tríade de nós no saco de cartões com uma soma dada.
 * 
 * Percorre a árvore em pré-ordem e busca por um par somando o complemento da
 * soma do nó atual. O tempo dessa busca é O(n^2 log n). Daria pra melhorar
 * sob as condições já levantadas na questão busca_par, mas não pareceu
 * necessário.
 * 
 * Quando a função encontra um resultado, retorna 1 e escreve os ponteiros para
 * os nós encontrados nos ponteiros `a`, `b` e `c`, passados por parâmetro.
 * 
 * @param s o saco de cartões.
 * @param raiz o nó atual do percurso em pré-ordem.
 * @param soma a soma sendo buscada.
 * @param a ponteiro de saída para um nó.
 * @param b ponteiro de saída para o outro nó.
 * @param b ponteiro de saída para o nó restante.
 * @return 1 caso encontre uma tríade, 0 caso contrário.
 */
int busca_triade(
    saco* s,
    node* raiz,
    int soma,
    node** a,
    node** b,
    node** c
) {
    if (raiz == NULL) {
        return 0;
    }
    
    if (busca_par(s, s->internal, soma - raiz->val.num, b, c)) {
        *a = raiz;
        return 1;
    }

    return busca_triade(s, raiz->esq, soma, a, b, c)
        || busca_triade(s, raiz->dir, soma, a, b, c);
}

/**
 * @brief Escreve a mensagem dos cartões percorrendo uma ABB em ordem.
 * 
 * @param raiz o nó raíz da árvore.
 */
void print_mensagem_rec(const node* raiz) {
    if (raiz->esq) {
        print_mensagem_rec(raiz->esq);
    }

    if (raiz->val.tipo == TOKEN_SIMPLES) {
        printf("%s", raiz->val.msg);
    } else {
        print_mensagem(&raiz->val.componentes);
    }

    if (raiz->dir) {
        print_mensagem_rec(raiz->dir);
    }
}

/**
 * @brief Libera os recursos alocados na árvore recursivamente.
 * 
 * @param raiz o nó raíz da árvore.
 */
void destroy_rec(node* raiz) {
    if (raiz == NULL) {
        return;
    }

    destroy_rec(raiz->esq);
    destroy_rec(raiz->dir);

    if (raiz->val.tipo == TOKEN_COMPOSTO) {
        destroy(&raiz->val.componentes);
    }

    free(raiz);
}

//=============================================================================
// Implementação (Contrato)
//=============================================================================

saco make_saco() {
    return (saco) { NULL };
}

void troca_triade(saco* s, int soma) {
    node *a, *b, *c;
    int achou = busca_triade(s, s->internal, soma, &a, &b, &c);
    assert(achou);

    token t = { soma, TOKEN_COMPOSTO, {{0}} };
    add_token_impl(&t.componentes, b->val);
    add_token_impl(&t.componentes, a->val);
    add_token_impl(&t.componentes, c->val);

    s->internal = remove_valor(s->internal, a->val.num);
    s->internal = remove_valor(s->internal, b->val.num);
    s->internal = remove_valor(s->internal, c->val.num);

    add_token_impl(s, t);
}

void add_token(saco* s, int num, const char msg[TOKEN_MSG_MAXLEN + 1]) {
    token t;
    t.num = num;
    t.tipo = TOKEN_SIMPLES;
    strcpy(t.msg, msg);

    add_token_impl(s, t);
}

void print_mensagem(const saco* s) {
    print_mensagem_rec(s->internal);
}

void destroy(saco* s) {
    destroy_rec(s->internal);
}
