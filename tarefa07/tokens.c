#include "tokens.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct saco_internal {
    token val;
    struct saco_internal* esq;
    struct saco_internal* dir;
} node;

node* novo_no(token t);
node* busca_pai(node* raiz, int num);
node* busca(node* raiz, int num);
node* insere(node* raiz, node* no);
node* remove_valor(node* raiz, int num);

node* novo_no(token t) {
    node* n = (node*) malloc(sizeof(node));
    n->val = t;
    n->esq = n->dir = NULL;
    return n;
}

node* busca_pai(node* raiz, int num) {
    node* anterior = NULL, *atual = raiz;

    while (atual != NULL) {
        int valor = atual->val.num;

        if (num == valor)
            break;

        anterior = atual;

        if (num < valor)
            atual = atual->esq;
        else // num > valor
            atual = atual->dir;
    }

    return anterior;
}

node* busca(node* raiz, int num) {
    node* anterior = busca_pai(raiz, num);

    if (anterior == NULL)
        return raiz;

    if (anterior->esq && num == anterior->esq->val.num)
        return anterior->esq;
    else if (anterior->dir && num == anterior->dir->val.num)
        return anterior->dir;
    else
        return anterior;
}

node* insere(node* raiz, node* no) {
    if (raiz == NULL)
        return no;

    int valor = no->val.num;
    node* anterior = busca(raiz, valor);

    // Assume que não existem duplicatas
    if (valor < anterior->val.num)
        anterior->esq = no;
    else
        anterior->dir = no;

    return raiz;
}

node* pop_max(node* raiz) {
    if (raiz->dir == NULL)
        return raiz;
    
    if (raiz->dir->dir == NULL) {
        node* rem = raiz->dir;
        raiz->dir = rem->esq;
        return rem;
    }

    return pop_max(raiz->dir); // Tailrec
}

node* pop_min(node* raiz) {
    if (raiz->esq == NULL)
        return raiz;
    
    if (raiz->esq->esq == NULL) {
        node* rem = raiz->esq;
        raiz->esq = rem->dir;
        return rem;
    }

    return pop_min(raiz->esq); // Tailrec
}

node* substitui(node* raiz) {
    if (!(raiz->dir || raiz->esq)) {
        return NULL;
    }

    node* novo;
    
    if (raiz->dir) {
        novo = pop_min(raiz->dir);
    } else {
        novo = pop_max(raiz->esq);
    }

    if (novo != raiz->dir)
        novo->dir = raiz->dir;

    if (novo != raiz->esq)
        novo->esq = raiz->esq;

    return novo;
}

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

void add_token(saco* s, token t) {
    node* novo = novo_no(t);
    s->internal = insere(s->internal, novo);
}

int busca_par(saco* s, node* raiz, node* ignora, int soma, node** a, node** b) {
    if (raiz == NULL) {
        return 0;
    } else {
        int subsoma = soma - raiz->val.num;

        node* no = busca(s->internal, subsoma);
        if (no != ignora && raiz != ignora && no->val.num == subsoma) {
            *a = raiz;
            *b = no;
            return 1;
        }

        return busca_par(s, raiz->esq, ignora, soma, a, b)
               || busca_par(s, raiz->dir, ignora, soma, a, b);
    }
}

int busca_triade_rec(saco* s, node* raiz, int soma, node** a, node** b, node** c) {
    if (raiz == NULL) {
        return 0;
    } else if (busca_par(s, s->internal, raiz, soma - raiz->val.num, b, c)) {
        *a = raiz;
        return 1;
    } else {
        return busca_triade_rec(s, raiz->esq, soma, a, b, c) ||
               busca_triade_rec(s, raiz->dir, soma, a, b, c);
    }
}

void troca_triade(saco* s, int soma) {
    node *a, *b, *c;
    busca_triade_rec(s, s->internal, soma, &a, &b, &c);

    s->internal = remove_valor(s->internal, a->val.num);
    s->internal = remove_valor(s->internal, b->val.num);
    s->internal = remove_valor(s->internal, c->val.num);

    token t = { soma, TOKEN_COMPOSTO, {{0}} };
    add_token(&t.componentes, b->val);
    add_token(&t.componentes, a->val);
    add_token(&t.componentes, c->val);

    add_token(s, t);
}

void print_mensagem_rec(node* raiz) {
    if (raiz->esq)
        print_mensagem_rec(raiz->esq);

    if (raiz->val.tipo == TOKEN_SIMPLES)
        printf("%s", raiz->val.msg);
    else
        print_mensagem(&raiz->val.componentes);

    if (raiz->dir)
        print_mensagem_rec(raiz->dir);
}

void print_mensagem(saco* s) {
    print_mensagem_rec(s->internal);
}
