#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct _node Node;
typedef struct _edge Edge;

typedef struct _node_vector NodeVector;
typedef struct _edge_vector EdgeVector;

typedef Node *(*NodeVectorCopyFunc)(const Node *elem);
typedef void (*NodeVectorFreeFunc)(Node *elem);
typedef Edge *(*EdgeVectorCopyFunc)(const Edge *elem);
typedef void (*EdgeVectorFreeFunc)(Edge *elem);

NodeVector *NodeVector_init(size_t init_cap, NodeVectorCopyFunc copy_func, NodeVectorFreeFunc free_func);
void NodeVector_destroy(NodeVector *vec);
size_t NodeVector_size(const NodeVector *vec);
size_t NodeVector_capacity(const NodeVector *vec);
int NodeVector_empty(const NodeVector *vec);
int NodeVector_reserve(NodeVector *vec, size_t new_cap);
int NodeVector_push_back(NodeVector *vec, const Node *elem);
int NodeVector_pop_back(NodeVector *vec);
int NodeVector_set(NodeVector *vec, size_t index, const Node *elem);
Node *NodeVector_get(const NodeVector *vec, size_t index);
void NodeVector_clear(NodeVector *vec);

EdgeVector *EdgeVector_init(size_t init_cap, EdgeVectorCopyFunc copy_func, EdgeVectorFreeFunc free_func);
void EdgeVector_destroy(EdgeVector *vec);
size_t EdgeVector_size(const EdgeVector *vec);
size_t EdgeVector_capacity(const EdgeVector *vec);
int EdgeVector_empty(const EdgeVector *vec);
int EdgeVector_reserve(EdgeVector *vec, size_t new_cap);
int EdgeVector_push_back(EdgeVector *vec, const Edge *elem);
int EdgeVector_pop_back(EdgeVector *vec);
int EdgeVector_set(EdgeVector *vec, size_t index, const Edge *elem);
Edge *EdgeVector_get(const EdgeVector *vec, size_t index);
void EdgeVector_clear(EdgeVector *vec);

#endif // VECTOR_H
