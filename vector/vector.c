#include "vector.h"
#include <stdlib.h>

#define DEFAULT_VEC_CAP 8

#define DEFINE_VECTOR(Type, Prefix, StructName, CopyFunc, FreeFunc)               \
  struct StructName                                                               \
  {                                                                               \
    size_t capacity;                                                              \
    size_t size;                                                                  \
                                                                                  \
    Type **data;                                                                  \
    CopyFunc copy_func;                                                           \
    FreeFunc free_func;                                                           \
  };                                                                              \
                                                                                  \
  static Type *Prefix##_copy_elem(const Prefix *vec, const Type *elem)            \
  {                                                                               \
    if (!vec->copy_func)                                                          \
      return (Type *)elem;                                                        \
                                                                                  \
    return vec->copy_func(elem);                                                  \
  }                                                                               \
                                                                                  \
  static void Prefix##_free_elem(Prefix *vec, Type *elem)                         \
  {                                                                               \
    if (vec->free_func)                                                           \
      vec->free_func(elem);                                                       \
  }                                                                               \
                                                                                  \
  static int Prefix##_grow(Prefix *vec)                                           \
  {                                                                               \
    if (!vec)                                                                     \
      return 0;                                                                   \
                                                                                  \
    size_t new_cap = vec->capacity ? vec->capacity * 2 : DEFAULT_VEC_CAP;         \
    return Prefix##_reserve(vec, new_cap);                                        \
  }                                                                               \
                                                                                  \
  Prefix *Prefix##_init(size_t init_cap, CopyFunc copy_func, FreeFunc free_func)  \
  {                                                                               \
    Prefix *vec = malloc(sizeof(Prefix));                                         \
    if (!vec)                                                                     \
      return NULL;                                                                \
                                                                                  \
    size_t cap = init_cap ? init_cap : DEFAULT_VEC_CAP;                           \
    *vec = (Prefix){cap, 0, calloc(cap, sizeof(Type *)), copy_func, free_func};   \
    if (!vec->data)                                                               \
    {                                                                             \
      free(vec);                                                                  \
      return NULL;                                                                \
    }                                                                             \
                                                                                  \
    return vec;                                                                   \
  }                                                                               \
                                                                                  \
  void Prefix##_destroy(Prefix *vec)                                              \
  {                                                                               \
    if (!vec)                                                                     \
      return;                                                                     \
                                                                                  \
    Prefix##_clear(vec);                                                          \
    free(vec->data);                                                              \
    free(vec);                                                                    \
  }                                                                               \
                                                                                  \
  size_t Prefix##_size(const Prefix *vec) { return vec ? vec->size : 0; }         \
                                                                                  \
  size_t Prefix##_capacity(const Prefix *vec) { return vec ? vec->capacity : 0; } \
                                                                                  \
  int Prefix##_empty(const Prefix *vec) { return !vec || vec->size == 0; }        \
                                                                                  \
  int Prefix##_reserve(Prefix *vec, size_t new_cap)                               \
  {                                                                               \
    if (!vec)                                                                     \
      return 0;                                                                   \
    if (new_cap <= vec->capacity)                                                 \
      return 1;                                                                   \
                                                                                  \
    Type **new_data = realloc(vec->data, new_cap * sizeof(Type *));               \
    if (!new_data)                                                                \
      return 0;                                                                   \
                                                                                  \
    for (size_t i = vec->capacity; i < new_cap; ++i)                              \
      new_data[i] = NULL;                                                         \
                                                                                  \
    vec->data = new_data;                                                         \
    vec->capacity = new_cap;                                                      \
    return 1;                                                                     \
  }                                                                               \
                                                                                  \
  int Prefix##_push_back(Prefix *vec, const Type *elem)                           \
  {                                                                               \
    if (!vec)                                                                     \
      return 0;                                                                   \
                                                                                  \
    if (vec->size == vec->capacity && !Prefix##_grow(vec))                        \
      return 0;                                                                   \
                                                                                  \
    Type *copy = Prefix##_copy_elem(vec, elem);                                   \
    if (!copy && elem)                                                            \
      return 0;                                                                   \
                                                                                  \
    vec->data[vec->size++] = copy;                                                \
    return 1;                                                                     \
  }                                                                               \
                                                                                  \
  int Prefix##_pop_back(Prefix *vec)                                              \
  {                                                                               \
    if (!vec || vec->size == 0)                                                   \
      return 0;                                                                   \
                                                                                  \
    Prefix##_free_elem(vec, vec->data[--vec->size]);                              \
    vec->data[vec->size] = NULL;                                                  \
    return 1;                                                                     \
  }                                                                               \
                                                                                  \
  int Prefix##_set(Prefix *vec, size_t index, const Type *elem)                   \
  {                                                                               \
    if (!vec || index >= vec->size)                                               \
      return 0;                                                                   \
                                                                                  \
    Type *copy = Prefix##_copy_elem(vec, elem);                                   \
    if (!copy && elem)                                                            \
      return 0;                                                                   \
    if (copy == vec->data[index])                                                 \
      return 1;                                                                   \
                                                                                  \
    Prefix##_free_elem(vec, vec->data[index]);                                    \
    vec->data[index] = copy;                                                      \
    return 1;                                                                     \
  }                                                                               \
                                                                                  \
  Type *Prefix##_get(const Prefix *vec, size_t index)                             \
  {                                                                               \
    if (!vec || index >= vec->size)                                               \
      return NULL;                                                                \
                                                                                  \
    return vec->data[index];                                                      \
  }                                                                               \
                                                                                  \
  void Prefix##_clear(Prefix *vec)                                                \
  {                                                                               \
    if (!vec)                                                                     \
      return;                                                                     \
                                                                                  \
    for (size_t i = 0; i < vec->size; ++i)                                        \
    {                                                                             \
      Prefix##_free_elem(vec, vec->data[i]);                                      \
      vec->data[i] = NULL;                                                        \
    }                                                                             \
    vec->size = 0;                                                                \
  }

DEFINE_VECTOR(Node, NodeVector, _node_vector, NodeVectorCopyFunc, NodeVectorFreeFunc)
DEFINE_VECTOR(Edge, EdgeVector, _edge_vector, EdgeVectorCopyFunc, EdgeVectorFreeFunc)
