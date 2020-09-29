/**
 * @file fakenews.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa03.html
 */

#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

//=============================================================================
// DEFINIÇÕES
//=============================================================================

// Tamanho máximo de um termo
#define TERMO_LEN_MAX 25

/**
 * TAD para uma categoria.
 */
typedef struct _categoria {
    int qtd_itens;  /* Número de itens atualmente na categoria */
    char* itens;    /* Lista de itens da categoria */
} Categoria_t;

/**
 * Inicializa uma categoria.
 * 
 * @param categoria ponteiro para a categoria (deve ser pré-alocada).
 * @param max_itens número máximo de elementos na categoria.
 */
void init_categoria(Categoria_t* categoria, int max_itens)
    __attribute__((
        nonnull(1)
#if __GNUC__ >= 10
        write_only(1)
#endif
    ));

/**
 * Libera a memória alocada para uma categoria.
 * 
 * @param categoria ponteiro para a categoria.
 */
void free_categoria(Categoria_t* categoria)
    __attribute__((nonnull(1)));

/**
 * Adiciona um item em uma categoria.
 * 
 * @param categoria pointeiro para a categoria (deve estar inicializado).
 * @param termo termo a ser adicionado.
 * 
 * @note a categoria não gerencia a memória para o parâmetro `termo`.
 */
void add_na_categoria(Categoria_t* categoria, const char* termo)
    __attribute__((
        nonnull(1)
#if __GNUC__ >= 10
        write_only(1),
        read_only(2)
#endif
    ));

/**
 * Obtém um pointeiro para um item em uma categoria.
 * 
 * @param categoria pointeiro para a categoria (deve estar inicializado).
 * @param indice índice do item desejado.
 */
char* item_na_categoria(const Categoria_t* categoria, int indice)
    __attribute__((nonnull(1)));

/**
 * Escreve os itens de uma categoria na saída padrão.
 * 
 * @param titulo título da categoria.
 * @param categoria pointeiro para a categoria (deve estar inicializado).
 */
void print_categoria(const char* titulo, const Categoria_t* categoria)
    __attribute__((nonnull(1, 2)));

/**
 * Lê um termo da entrada padrão e seu histórico e gera a saída para o
 * programa, além de adicionar o termo à categoria apropriada.
 * 
 * @param categorias lista de categorias (deve estar inicializada).
 * @param n_hist Número de elementos no histórico.
 */
void processar_termo(Categoria_t categorias[], int n_hist)
    __attribute__((
        nonnull(1)
#if __GNUC__ >= 10
        write_only(1)
#endif
    ));

/**
 * Enumeração de categorias.
 */
typedef enum _categorias {
    BOT,
    SURPREENDENTE,
    NORMAL,
    LOCAL,
    IRRELEVANTE
} Categorias;

/**
 * Classifica um termo em uma categoria dado suas estatísticas.
 * 
 * @param max relevância máxima do termo.
 * @param min relevância mínima do termo.
 * @param media média aritmética de relevância do termo.
 * @param dp desvio padrão da relevância do termo.
 */
Categorias classificar(double max, double min, double media, double dp)
    __attribute_const__;

//=============================================================================
// PONTO DE ENTRADA
//=============================================================================

int main() {
    int n, m;
    scanf("%d %d", &n, &m);

    Categoria_t* categorias = (Categoria_t*) malloc(5 * sizeof(Categoria_t));
    for (int i = 0; i < 5; i++) init_categoria(&categorias[i], n);

    for (int i = 0; i < n; i++) processar_termo(categorias, m);

    printf("\nRESULTADO:\n");
    print_categoria("Bot", &categorias[BOT]);
    print_categoria("Surpreendente", &categorias[SURPREENDENTE]);
    print_categoria("Normal", &categorias[NORMAL]);
    print_categoria("Local", &categorias[LOCAL]);
    print_categoria("Irrelevante", &categorias[IRRELEVANTE]);

    for (int i = 0; i < 5; i++) free_categoria(&categorias[i]);
    free(categorias);

    return 0;
}

//=============================================================================
// IMPLEMENTAÇÕES (Categoria)
//=============================================================================

void init_categoria(Categoria_t* categoria, int max_items) {
    categoria->qtd_itens = 0;

    // Os itens são sequenciais na memória, assim fica mais fácil de liberar
    // depois.
    categoria->itens = malloc(max_items * (TERMO_LEN_MAX + 1) * sizeof(char));
}

void free_categoria(Categoria_t* categoria) {
    free(categoria->itens);
}

void add_na_categoria(Categoria_t* categoria, const char* termo) {
    char* dest = item_na_categoria(categoria, categoria->qtd_itens);
    strcpy(dest, termo);

    categoria->qtd_itens++;
}

char* item_na_categoria(const Categoria_t* categoria, int indice) {
    return &categoria->itens[(TERMO_LEN_MAX + 1) * indice];
}

void print_categoria(const char* titulo, const Categoria_t* categoria) {
    printf("%s (%d):", titulo, categoria->qtd_itens);
    for (int i = 0; i < categoria->qtd_itens; i++) {
        const char* termo = item_na_categoria(categoria, i);
        printf(" %s", termo);
    }
    printf("\n");
}

//=============================================================================
// IMPLEMENTAÇÕES (Fake News)
//=============================================================================

Categorias classificar(double max, double min, double media, double dp) {
    if (media >= 60) {
        if (dp > 15) {
            return BOT;
        } else {
            return SURPREENDENTE;            
        }
    } else if (max >= 80) {
        if (min > 20) {
            return NORMAL;
        } else {
            return LOCAL;
        }
    } else {
        return IRRELEVANTE;
    }
}

void calc_estatisticas(
    int n_hist,
    double* max,
    double* min,
    double* media,
    double* dp
) {
    *min = INFINITY;
    *max = -INFINITY;
    *media = 0;

    double m2 = 0;
    for (int i = 0; i < n_hist; i++) {
        double val;
        scanf("%lf", &val);

        if (val < *min) *min = val;
        if (val > *max) *max = val;

        // Algoritmo de Welford para cálculo de variância e média
        double delta = val - *media;
        *media += delta / (i + 1);

        double delta2 = val - *media;
        m2 += delta * delta2;
    }

    *dp = sqrt(m2 / n_hist);
}

void processar_termo(Categoria_t categorias[], int n_hist) {
    char* termo = alloca(26 * sizeof(char));
    scanf("%s", termo);

    double min, max, media, dp;
    calc_estatisticas(n_hist, &max, &min, &media, &dp);

    printf("%s %.2lf %.2lf %.2lf %.2lf\n", termo, max, min, media, dp);
    
    Categorias cat = classificar(max, min, media, dp);
    add_na_categoria(&categorias[cat], termo);
}
