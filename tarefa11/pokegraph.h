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

graph_t* make_graph();

void add_waypoint(graph_t* g, point_t position, waypoint_type type);

double minimum_greatest_interval(graph_t* g, point_t origin);

void destroy_graph(graph_t* g);

#endif // __POKEGRAPH_H__
