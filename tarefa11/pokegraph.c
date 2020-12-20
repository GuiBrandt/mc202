/**
 * @file pokegraph.c
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Implementação das funções do grafo de paradas do Pokemon GO.
 */

#include "pokegraph.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <math.h>
#include <assert.h>

#include "xmalloc.h"
#include "pokedisjointset.h"

//=============================================================================
// DEFINIÇÕES
//=============================================================================

/**
 * @brief Estrutura das paradas. 
 */
typedef struct {
    point_t position;
    waypoint_type type;
} waypoint_t;

/**
 * @brief Estrutura do grafo.
 * 
 * Como é um grafo completo, a estrutura é apenas um array dinâmico de paradas.
 */
struct graph {
    size_t capacity;
    size_t size;
    waypoint_t* waypoints;
};

// Capacidade inicial do array de vértices do grafo
#define INITIAL_CAPACITY 128

/**
 * @brief Estrutura de uma aresta no grafo.
 * 
 * Esta estrutura é usada exclusivamente no algoritmo de caminho com menor
 * distância entre as paradas.
 */
typedef struct edge {
    int a, b;
    double weight;
} edge;

//=============================================================================
// IMPLEMENTAÇÃO (Auxiliares)
//=============================================================================

/**
 * @brief Encontra um nó no grafo dado o ponto que ele representa.
 * 
 * Esta função assume que existe um nó no grafo que representa o ponto dado.
 * Caso contrário, seu comportamento é indeterminado.
 * 
 * @param g o grafo.
 * @param point o ponto.
 * 
 * @return o índice do nó desejado na lista de nós do grafo. 
 */
int find_node(const graph_t* g, point_t point) {
    for (int i = 0; i < g->size; i++) {
        point_t v = g->waypoints[i].position;
        if (v.x == point.x && v.y == point.y) {
            return i;
        }
    }

    __builtin_unreachable();
}

/**
 * @brief (Quadrado da) distância euclidiana entre dois pontos.
 * 
 * A função calcula apenas o quadrado da distância para evitar calcular a raíz,
 * que é uma operação custosa.
 * 
 * @param a um ponto.
 * @param b outro ponto.
 * 
 * @return o quadrado da distância euclidiana entre os pontos dados. 
 */
double d2(point_t a, point_t b) {
    double dx = a.x - b.x,
           dy = a.y - b.y;

    return dx * dx + dy * dy;
}

/**
 * @brief Constrói todas as arestas de um grafo completo em um vetor de saída.
 * 
 * @param g o grafo.
 * @param edges o vetor de saída das arestas.
 */
void build_all_edges(const graph_t* g, edge* edges) {
    int k = 0;

    for (int i = 0; i < g->size; i++) {
        for (int j = 0; j < i; j++) {
            edges[k].a = i;
            edges[k].b = j;

            point_t a = g->waypoints[i].position,
                    b = g->waypoints[j].position;
            edges[k].weight = d2(a, b);

            k++;
        }
    }
}

/**
 * @brief Compara duas arestas no grafo por peso.
 * 
 * @param a uma aresta.
 * @param b outra aresta.
 * 
 * @return um inteiro < 0 se a < b, 0 se forem iguais ou > 0 se a > b
 */
static int compare_edges(const void* a, const void* b) {
    const edge  *e = (const edge*) a,
                *f = (const edge*) b;

    if (e->weight < f->weight) {
        return -1;
    } else if (e->weight > f->weight) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Adiciona arestas em um grafo até que exista um caminho entre o ponto
 *        dado e um Lugia.
 * 
 * Esta função constrói uma estrutura de união-busca (disjoint-set) no grafo,
 * aumentada para aceitar "marcações" em conjuntos.
 * 
 * O algoritmo percorre a lista ordenada de arestas e mescla os conjuntos dos
 * vértices conectados por cada aresta. Quando um vértice é uma parada com um
 * Lugia, o conjunto é marcado. Quando o ponto inicial é mesclado com um
 * conjunto marcado, sabemos que existe um caminho dele até algum Lugia, e o
 * algoritmo termina.
 * 
 * Como a lista de arestas é ordenada, sabemos que a última aresta adicionada é
 * a maior no caminho e menor que todas as outras que ainda não foram
 * adicionadas ao conjunto (e portanto certamente não fazem parte do caminho).
 * Logo, o peso dela dá a resposta da tarefa.
 * 
 * A complexidade de tempo deste algoritmo é O(|E| α(|V|)), onde |V| = número
 * de vértices no grafo, |E| = número de arestas e α é a função inversa de
 * Ackermann, referente ao tempo tomado nas operações de união e busca na
 * estrutura de união-busca. Efetivamente, o algoritmo toma tempo O(|E|), visto
 * que α(|V|) é menor que 5 para qualquer valor razoável de |V|.
 * 
 * @param g o grafo.
 * @param start o índice do vértice inicial no grafo.
 * @param n_edges o número de arestas na lista.
 * @param edges lista de arestas no grafo.
 * 
 * @return o índice da última (i.e. a maior) aresta adicionada.
 */
int add_edges_until_find_lugia(
    const graph_t* g,
    int start,
    int n_edges,
    edge* edges
) {
    qsort(edges, n_edges, sizeof(edge), compare_edges);

    disjoint_set_t* ds = make_disjoint_set(g->size);

    for (int i = 0; i < n_edges; i++) {
        edge e = edges[i];

        disjoint_set_merge(ds, e.a, e.b);

        waypoint_t w_a = g->waypoints[e.a],
                    w_b = g->waypoints[e.b];

        if (w_a.type == LUGIA || w_b.type == LUGIA) {
            disjoint_set_mark(ds, e.a);
        }

        if (disjoint_set_marked(ds, disjoint_set_find(ds, start))) {
            destroy_disjoint_set(ds);
            return i;
        }
    }

    // Sempre deve existir um caminho: no mínimo, existe o caminho que vai
    // direto do ponto inicial até o Lugia.
    __builtin_unreachable();
}

//=============================================================================
// IMPLEMENTAÇÃO (Contrato)
//=============================================================================

graph_t* make_graph() {
    graph_t* g = (graph_t*) xmalloc(sizeof(graph_t));

    g->size = 0;
    g->capacity = INITIAL_CAPACITY;
    g->waypoints = (waypoint_t*) xmalloc(sizeof(waypoint_t) * INITIAL_CAPACITY);

    return g;
}

// Tempo: O(1) amortizado
void add_waypoint(graph_t* g, point_t position, waypoint_type type) {
    if (++g->size > g->capacity) {
        g->capacity *= 2;
        g->waypoints = (waypoint_t*) realloc(g->waypoints, sizeof(waypoint_t) * g->capacity);

        if (g->waypoints == NULL) {
            fprintf(stderr, "Erro fatal: Out of memory.");
            exit(-1);
        }
    }

    g->waypoints[g->size - 1] = (waypoint_t) { position, type };
}

// Tempo: O(|E| log |E|) = O(|E| log |V|), visto que |E| = |V|^2. Referente ao
//        tempo de ordenar as arestas por tamanho, que domina as demais
//        operações.
//
// Espaço: O(|E|) = O(|V|^2)
double minimum_greatest_interval(const graph_t* g, point_t origin) {
    int start = find_node(g, origin);

    // Alocamos C(n, 2) = (n^2 - n) / 2 arestas, referente ao número de arestas
    // únicas no grafo completo não-direcionado.
    int n_edges = g->size * (g->size - 1) / 2;
    edge* edges = (edge*) xmalloc(sizeof(edge) * n_edges);
    build_all_edges(g, edges);

    int i_best = add_edges_until_find_lugia(g, start, n_edges, edges);
    double best = edges[i_best].weight;

    free(edges);

    return sqrt(best);
}

void destroy_graph(graph_t* g) {
    free(g->waypoints);
    free(g);
}
