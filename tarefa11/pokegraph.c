#include "pokegraph.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <math.h>
#include <assert.h>

#include "xmalloc.h"
#include "pokequeue.h"
#include "pokedisjointset.h"

/**
 * @brief Estrutura das paradas. 
 */
typedef struct {
    point_t position;
    waypoint_type type;
} waypoint_t;

struct graph {
    size_t capacity;
    size_t size;
    waypoint_t* waypoints;
};

#define INITIAL_CAPACITY 128

int find_node(graph_t* g, point_t point) {
    for (int i = 0; i < g->size; i++) {
        point_t v = g->waypoints[i].position;
        if (v.x == point.x && v.y == point.y) {
            return i;
        }
    }

    __builtin_unreachable();
}

double d2(point_t a, point_t b) {
    double dx = a.x - b.x,
           dy = a.y - b.y;

    return dx * dx + dy * dy;
}

int closest_nonvisited(graph_t* g, bool* visited, int source, double* distance) {
    int argmin = -1;
    double min = (double) INFINITY;

    point_t s = g->waypoints[source].position;
    
    for (int i = 0; i < g->size; i++) {
        if (i == source || visited[i]) {
            continue;
        }

        point_t v = g->waypoints[i].position;
        double d = d2(s, v);

        if (d < min) {
            min = d;
            argmin = i;
        }
    }

    *distance = min;
    return argmin;
}

graph_t* make_graph() {
    graph_t* g = (graph_t*) xmalloc(sizeof(graph_t));

    g->size = 0;
    g->capacity = INITIAL_CAPACITY;
    g->waypoints = (waypoint_t*) xmalloc(sizeof(waypoint_t) * INITIAL_CAPACITY);

    return g;
}

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

bool better(graph_t* g, int from, int to_current, int to_alt) {
    point_t p1 = g->waypoints[from].position,
            p2 = g->waypoints[to_current].position,
            p3 = g->waypoints[to_alt].position;

    return d2(p1, p3) < d2(p1, p2); 
}

typedef struct edge {
    int a, b;
    double weight;
} edge;

static int compare(const void* a, const void* b) {
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

double minimum_greatest_interval(graph_t* g, point_t origin) {
    edge* all_pairs_edges = (edge*) xmalloc(sizeof(edge) * g->size * g->size);

    int k = 0;
    for (int i = 0; i < g->size; i++) {
        for (int j = 0; j < i; j++) {
            all_pairs_edges[k].a = i;
            all_pairs_edges[k].b = j;
            all_pairs_edges[k].weight = d2(g->waypoints[i].position, g->waypoints[j].position);
            k++;
        }
    }

    int start = find_node(g, origin);

    qsort(all_pairs_edges, k, sizeof(edge), compare);

    disjoint_set_t* ds = make_disjoint_set(g->size);

    int i;
    for (i = 0; i < k; i++) {
        edge e = all_pairs_edges[i];

        merge(ds, e.a, e.b);

        waypoint_t w_a = g->waypoints[e.a],
                    w_b = g->waypoints[e.b];

        if (w_a.type == LUGIA || w_b.type == LUGIA) {
            mark(ds, e.a);
        }

        if (marked(ds, find(ds, start))) {
            break;
        }
    }

    double best = all_pairs_edges[i].weight;

    destroy_disjoint_set(ds);
    free(all_pairs_edges);

    return sqrt(best);
}

void destroy_graph(graph_t* g) {
    free(g->waypoints);
    free(g);
}
