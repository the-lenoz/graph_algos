#ifndef ALGO_H
#define ALGO_H
#include "../htable/htable.h"
#include "../vector/vector.h"
#include <stdint.h>

typedef struct _id_list
{
  uint64_t val;
  struct _id_list *next;
} IdList;

struct _node
{
  int alive;
  char *name;
  IdList *entering_edges;
  IdList *exiting_edges;
};

struct _edge
{
  int alive;
  uint64_t src_id;
  uint64_t dst_id;
  int weight;
};

typedef struct
{
  HTable *node_names_map;

  NodeVector *nodes;
  EdgeVector *edges;
} Graph;

Graph *Graph_init();
void Graph_destroy(Graph *g);

int Graph_add_node(Graph *g, const char *node_name);
int Graph_remove_node(Graph *g, const char *node_name);

int Graph_add_edge(Graph *g, const char *src_name, const char *dst_name, int weight);
int Graph_remove_edge(Graph *g, const char *src_name, const char *dst_name);

int Graph_run_RPO(Graph *g, const char *start_node_name);
int Graph_run_Dijkstra(Graph *g, const char *node_name);
int Graph_run_max_flow(Graph *g, const char *src_name, const char *dst_name);
int Graph_run_Tarjan(Graph *g, const char *node_name);


#endif // ALGO_H
