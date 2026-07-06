#include "algo.h"
#include "../htable/htable.h"
#include "../vector/vector.h"
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAP 64

static void IdList_destroy(IdList *l)
{
  if (!l)
    return;
  IdList *next = l->next;
  free(l);
  IdList_destroy(next);
}

static void IdList_append(IdList **l, int64_t val)
{
  if (!l)
    return;
  if (!*l)
  {
    *l = malloc(sizeof(IdList));
    if (!*l)
      return;
    **l = (IdList){val, NULL};
    return;
  }
  IdList_append(&(*l)->next, val);
}
static int64_t IdList_pop(IdList **l)
{
  if (!l || !*l)
    return 0;
  if (!(*l)->next)
  {
    int64_t val = (*l)->val;
    free(*l);
    *l = NULL;
    return val;
  }
  return IdList_pop(&(*l)->next);
}

static IdList *IdList_copy(const IdList *l)
{
  if (!l)
    return NULL;
  IdList *copy = malloc(sizeof(IdList));
  if (!copy)
    return NULL;
  copy->val = l->val;
  copy->next = IdList_copy(l->next);
  return copy;
}

static int64_t IdList_popitem(IdList **elem)
{
  if (!elem || !*elem)
    return 0;

  IdList *e = *elem;
  *elem = e->next;
  int64_t data = e->val;
  free(e);
  return data;
}

static int is_valid_node_name(const char *name)
{
  if (!name || !isalpha(*name))
    return 0;
  do
    if (!isdigit(*name) && !isalpha(*name))
      return 0;
  while (*++name);

  return 1;
}

static int remove_edge_by_index(Graph *g, uint64_t edge_index)
{
  if (!g || edge_index >= EdgeVector_size(g->edges))
    return 0;

  Edge *edge = EdgeVector_get(g->edges, edge_index);

  if (!edge)
    return 0;

  uint64_t src_id = edge->src_id, dst_id = edge->dst_id;

  Node *src_node = NodeVector_get(g->nodes, src_id),
       *dst_node = NodeVector_get(g->nodes, dst_id);

  if (!src_node || !dst_node)
    return 0;

  for (IdList **edge = &src_node->exiting_edges; *edge; edge = &(*edge)->next)
    if ((*edge)->val == edge_index)
    {
      IdList_popitem(edge);
      break;
    }

  for (IdList **edge = &dst_node->entering_edges; *edge; edge = &(*edge)->next)
    if ((*edge)->val == edge_index)
    {
      IdList_popitem(edge);
      break;
    }

  edge->alive = 0;

  return 1;
}

static void destroy_node(Node *node)
{
  if (!node)
    return;
  IdList_destroy(node->entering_edges);
  IdList_destroy(node->exiting_edges);
  node->alive = 0;
  free(node->name);
  free(node);
}
static Node *node_copy(const Node *node)
{
  if (!node)
    return NULL;
  Node *copy = malloc(sizeof(Node));
  if (!copy)
    return NULL;
  *copy = (Node){
      node->alive, strdup(node->name),
      IdList_copy(node->entering_edges), IdList_copy(node->exiting_edges)};
  return copy;
}

static void destroy_edge(Edge *edge)
{
  free(edge);
}

static Edge *edge_copy(const Edge *edge)
{
  if (!edge)
    return NULL;
  Edge *copy = malloc(sizeof(Edge));
  if (!copy)
    return NULL;
  *copy = *edge;
  return copy;
}

Graph *Graph_init()
{
  Graph *g = malloc(sizeof(Graph));
  if (!g)
    return NULL;

  g->node_names_map = ht_init(DEFAULT_CAP);
  if (!g->node_names_map)
    return Graph_destroy(g), NULL;
  g->nodes = NodeVector_init(DEFAULT_CAP, node_copy, destroy_node);
  if (!g->nodes)
    return Graph_destroy(g), NULL;
  g->edges = EdgeVector_init(DEFAULT_CAP, edge_copy, destroy_edge);
  if (!g->edges)
    return Graph_destroy(g), NULL;

  return g;
}

void Graph_destroy(Graph *g)
{
  if (!g)
    return;
  ht_destroy(g->node_names_map);
  NodeVector_destroy(g->nodes);
  EdgeVector_destroy(g->edges);
  free(g);
}

int Graph_add_node(Graph *g, const char *node_name)
{
  if (!g || !is_valid_node_name(node_name))
    return 0;

  uint64_t id;
  if (ht_get(g->node_names_map, node_name, (int64_t *)&id))
    return 0; // Node already exists

  id = NodeVector_size(g->nodes);
  struct _node node = (Node){
      1,
      (char *)node_name,
      NULL,
      NULL};

  if (!NodeVector_push_back(g->nodes, &node) || !ht_set(g->node_names_map, node_name, id))
    return 0; // Can't add node

  return 1;
}

int Graph_remove_node(Graph *g, const char *node_name)
{
  uint64_t id;
  if (!g || !is_valid_node_name(node_name))
    return 0;
  if (!ht_get(g->node_names_map, node_name, (int64_t *)&id))
    return printf("Unknown node %s\n", node_name), 0;

  struct _node *node = NodeVector_get(g->nodes, id);
  if (!node->alive)
    return 0;

  for (IdList *edge = node->entering_edges, *next = edge ? edge->next : NULL;
       edge; edge = next, next = next ? next->next : NULL)
    remove_edge_by_index(g, edge->val);

  for (IdList *edge = node->exiting_edges, *next = edge ? edge->next : NULL;
       edge; edge = next, next = next ? next->next : NULL)
    remove_edge_by_index(g, edge->val);

  ht_unset(g->node_names_map, node_name);
  node->alive = 0;

  return 1;
}

int Graph_add_edge(Graph *g, const char *src_name, const char *dst_name, int weight)
{
  if (!g || !is_valid_node_name(src_name) || !is_valid_node_name(dst_name))
    return 0;

  uint64_t src_id = 0, dst_id = 0;
  int found = 0b11;
  if (!ht_get(g->node_names_map, src_name, (int64_t *)&src_id))
    found &= 0b10;

  if (!ht_get(g->node_names_map, dst_name, (int64_t *)&dst_id))
    found &= 0b01;

  Node *src_node = NodeVector_get(g->nodes, src_id),
       *dst_node = NodeVector_get(g->nodes, dst_id);

  if (!src_node || !src_node->alive)
    found &= 0b10;
  if (!dst_node || !dst_node->alive)
    found &= 0b01;

  if (found == 0b00)
    return printf("Unknown nodes %s %s\n", src_name, dst_name), 0;
  else if (found < 0b11)
    return printf("Unknown node %s\n", found == 0b01 ? dst_name : src_name), 0;

  Edge edge = (Edge){1, src_id, dst_id, weight};
  uint64_t edge_id = EdgeVector_size(g->edges);
  EdgeVector_push_back(g->edges, &edge);

  IdList_append(&src_node->exiting_edges, edge_id);
  IdList_append(&dst_node->entering_edges, edge_id);

  return 1;
}
int Graph_remove_edge(Graph *g, const char *src_name, const char *dst_name)
{
  if (!g || !is_valid_node_name(src_name) || !is_valid_node_name(dst_name))
    return 0;

  uint64_t src_id = 0, dst_id = 0;
  int found = 0b11;
  if (!ht_get(g->node_names_map, src_name, (int64_t *)&src_id))
    found &= 0b10;

  if (!ht_get(g->node_names_map, dst_name, (int64_t *)&dst_id))
    found &= 0b01;

  Node *src_node = NodeVector_get(g->nodes, src_id),
       *dst_node = NodeVector_get(g->nodes, dst_id);

  if (!src_node || !src_node->alive)
    found &= 0b10;
  if (!dst_node || !dst_node->alive)
    found &= 0b01;

  if (found == 0b00)
    return printf("Unknown nodes %s %s\n", src_name, dst_name), 0;
  else if (found < 0b11)
    return printf("Unknown node %s\n", found == 0b01 ? dst_name : src_name), 0;

  for (IdList *edge_l = src_node->exiting_edges; edge_l; edge_l = edge_l->next)
  {
    Edge *edge = EdgeVector_get(g->edges, edge_l->val);
    if (edge->dst_id == dst_id)
    {
      return remove_edge_by_index(g, edge_l->val);
    }
  }
  return 0;
}

static int postorder_rec(Graph *g, uint64_t node_id, IdList **postorder, int *visited)
{
  if (!g)
    return 0;
  Node *node = NodeVector_get(g->nodes, node_id);
  if (!node || !node->alive || visited[node_id])
    return 0;

  visited[node_id] = 1;
  
  for (IdList *edge_l = node->exiting_edges; edge_l; edge_l = edge_l->next)
  {
    Edge *edge = EdgeVector_get(g->edges, edge_l->val);
    if (!edge)
      continue;
    postorder_rec(g, edge->dst_id, postorder, visited);
  }
  IdList_append(postorder, node_id);
  return 1;
}

int Graph_run_RPO(Graph *g, const char *start_node_name)
{
  uint64_t node_id;
  if (!g || !ht_get(g->node_names_map, start_node_name, (int64_t *)&node_id))
    return 0;

  int size = NodeVector_size(g->nodes);
  IdList *postorder = NULL;
  int *visited = calloc(size, sizeof(int));
  if (!postorder_rec(g, node_id, &postorder, visited))
    return 0;

  free(visited);
  
  int *RPO_index = calloc(size, sizeof(int));
  for (int i = 0; i < size; ++i)
    RPO_index[i] = INT_MAX;
  int i = 0;
  IdList *RPO = NULL;
  while (postorder)
  {
    uint64_t node_id = (uint64_t)IdList_pop(&postorder);
    IdList_append(&RPO, node_id);
    RPO_index[node_id] = i++;
  }

  for (int i = 0; i < size; ++i)
  {
    Node *src_node = NodeVector_get(g->nodes, i);
    if (!src_node->alive)
      continue;
    for (IdList *edge_l = src_node->exiting_edges; edge_l; edge_l = edge_l->next)
    {
      Edge *edge = EdgeVector_get(g->edges, edge_l->val);
      if (RPO_index[i] > RPO_index[edge->dst_id])
      {
        Node *dst_node = NodeVector_get(g->nodes, edge->dst_id);
        printf("Found loop %s->%s\n", src_node->name, dst_node->name);
      }
    }
  }
  free(RPO_index);

  for (IdList *node_l = RPO; node_l; node_l = node_l->next)
  {
    Node *node = NodeVector_get(g->nodes, node_l->val);
    printf("%s ", node->name);
  }
  printf("\n");
  IdList_destroy(RPO);

  return 1;
}

int Graph_run_Dijkstra(Graph *g, const char *node_name)
{
  return 0;
}    
