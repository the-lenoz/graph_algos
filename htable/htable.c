#include "htable.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_HT_CAP 64

struct _hte
{
  char *k;
  int64_t val;
  _htelem *next;
};

struct _htable
{
  size_t capacity;
  size_t size;

  _htelem **elems;
};

static size_t str_hash(const void *k)
{
  const unsigned char *str = (const unsigned char *)k;
  size_t hash = 5381;
  int c;

  while ((c = *str++))
  {
    // hash = hash * 33 + c
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

static void *str_k_dup(const void *k) { return k ? strdup(k) : NULL; }

static inline size_t _ht_getindex(HTable *ht, const void *k)
{
  return str_hash(k) % ht->capacity;
}

static _htelem **_move_insert_before(_htelem **elem, _htelem **dst_elem)
{
  if (*elem == *dst_elem)
    return &(*elem)->next;

  _htelem *next = (*elem)->next;

  (*elem)->next = *dst_elem;
  *dst_elem = *elem;
  *elem = next;
  return elem;
}

static int _ht_rehash(HTable *ht)
{
  if (!ht || ht->size * 10 < ht->capacity * 7)
    return 1;

  size_t new_cap = ht->capacity * 2 * sizeof(_htelem *);

  _htelem **new_elems = realloc(ht->elems, new_cap);
  if (!new_elems)
    return 0;

  ht->elems = new_elems;
  memset(ht->elems + ht->capacity, 0, ht->capacity * sizeof(_htelem *));

  ht->capacity *= 2;
  for (_htelem **bucket = ht->elems; (void *)bucket < (void *)(ht->elems) + new_cap; ++bucket)
    for (_htelem **e = bucket; *e; e = _move_insert_before(e, &(ht->elems[_ht_getindex(ht, (*e)->k)])))
      ;
  return 1;
}

HTable *ht_init(size_t init_cap)
{
  HTable *ht = malloc(sizeof(HTable));
  if (!ht)
    return NULL;

  size_t cap = init_cap ? init_cap : DEFAULT_HT_CAP;
  *ht = (HTable){
      cap, 0, calloc(cap, sizeof(_htelem *))};
  if (!ht->elems)
  {
    free(ht);
    return NULL;
  }

  return ht;
}
void ht_destroy(HTable *ht)
{
  if (!ht)
    return;

  if (ht->elems)
  {
    _htelem *c, *n = NULL;
    for (size_t i = 0; i < ht->capacity; ++i)
    {
      for (c = ht->elems[i]; c; c = n)
      {
        n = c->next;
        free(c->k);
        free(c);
      }
    }
    free(ht->elems);
  }
  free(ht);
}

int ht_set(HTable *ht, const char *k, int64_t v)
{
  if (!ht || !k)
    return 0;
  size_t h = _ht_getindex(ht, k);
  _htelem *e;
  for (e = ht->elems[h]; e; e = e->next)
  {
    if (!strcmp(k, e->k))
    {
      char *k_copy = str_k_dup(k);
      if (!k_copy)
        return 0;

      free(e->k);
      e->k = k_copy;
      e->val = v;
      return 1;
    }
  }
  if (!_ht_rehash(ht))
    return 0;

  h = _ht_getindex(ht, k);
  e = malloc(sizeof(_htelem));
  if (!e)
    return 0;

  char *k_copy = str_k_dup(k);
  if (!k_copy)
  {
    free(e);
    return 0;
  }

  *e = (_htelem){k_copy, v, ht->elems[h]};
  ht->elems[h] = e;
  ht->size++;
  return 1;
}

int ht_get(HTable *ht, const char *k, int64_t *dest)
{
  if (!ht || !k || !dest)
    return 0;
  size_t h = str_hash(k) % ht->capacity;

  for (_htelem *e = ht->elems[h]; e; e = e->next)
    if (!strcmp(e->k, k))
      return *dest = e->val, 1;
  return 0;
}

int ht_unset(HTable *ht, const char *k)
{
  if (!ht || !k)
    return 0;

  size_t h = _ht_getindex(ht, k);
  for (_htelem **e = &ht->elems[h]; *e; e = &(*e)->next)
  {
    if (!strcmp((*e)->k, k))
    {
      _htelem *removed = *e;
      *e = removed->next;
      free(removed->k);
      free(removed);
      ht->size--;
      return 1;
    }
  }

  return 0;
}
