/**
 * @file pokegraph.h
 * 
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definições dos tipos abstratos de dados relacionados ao grafo dos
 *        pontos de interesse do Pokemon GO.
 */

#ifndef __POKEGRAPH_H__
#define __POKEGRAPH_H__

/**
 * @brief Estrutura de ponto cartesiano. 
 */
typedef struct {
    double x;
    double y;
} point_t;

/**
 * @brief Enumeração de tipos de parada. 
 */
typedef enum {
    POKESTOP,
    LUGIA
} waypoint_type;

/**
 * @brief Estrutura opaca de grafo. 
 */
typedef struct graph graph_t;

/**
 * @brief Aloca e inicializa um grafo vazio.
 * 
 * @return o grafo alocado.
 */
graph_t* make_graph();

/**
 * @brief Adiciona uma parada no grafo.
 * 
 * @param g o grafo.
 * @param position a posição da parada.
 * @param type o tipo de parada (POKESTOP/LUGIA).
 */
void add_waypoint(graph_t* g, point_t position, waypoint_type type);

/**
 * @brief Calcula a menor distância que um jogador saindo de uma origem
 *        consegue ficar sem jogar em um caminho até um Lugia no grafo.
 * 
 * @param g o grafo.
 * @param origin o ponto de origem o jogador.
 * 
 * @return a distância entre os pontos mais distantes no caminho no grafo que
 *         minimiza a distância entre cada ponto de parada do jogador até o
 *         Lugia.
 */
double minimum_greatest_interval(const graph_t* g, point_t origin);

/**
 * @brief Libera os recursos alocados para um grafo.
 * 
 * @param g o grafo.
 */
void destroy_graph(graph_t* g);

#endif // __POKEGRAPH_H__
