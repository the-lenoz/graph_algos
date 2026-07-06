#include "heap.h"
#include <stdlib.h>

#define MIN_HEAP_CAP 64

struct _heap
{
  size_t size;
  size_t capacity;
  HeapItem *items;
};

Heap *Heap_init()
{
  Heap *heap = malloc(sizeof(Heap));
  if (!heap)
    return NULL;
  heap->size = 0;
  heap->capacity = MIN_HEAP_CAP;
  heap->items = malloc(MIN_HEAP_CAP * sizeof(HeapItem));
  if (!heap->items)
    return free(heap), NULL;
  return heap;
}
void Heap_destroy(Heap *heap)
{
  if (!heap)
    return;
  free(heap->items);
  free(heap);
}

static int adjust_heap_capacity(Heap *heap)
{
  if (!heap)
    return 0;
  size_t new_cap = heap->capacity;
  
  if (heap->size + 1 >= heap->capacity)
    new_cap = heap->capacity * 2;
  else if (heap->size + 1 <= heap->capacity / 2 && heap->capacity >= MIN_HEAP_CAP * 2)
    new_cap = heap->capacity / 2;

  if (heap->capacity != new_cap)
  {
    HeapItem *new_items = realloc(heap->items, sizeof(HeapItem) * new_cap);
    if (!new_items)
      return 0;
    heap->items = new_items;
    heap->capacity = new_cap;
  }
  return 1;
}

static int sift_up(Heap *heap, size_t index)
{
  if (!heap)
    return 0;
  if (index == 0)
    return 1;
  size_t parent_index = (index - 1) / 2;
  if (heap->items[index].val >= heap->items[parent_index].val)
    return 1;
  HeapItem tmp = heap->items[parent_index];
  heap->items[parent_index] = heap->items[index];
  heap->items[index] = tmp;
  return sift_up(heap, parent_index);
}

static int sift_down(Heap *heap, size_t index)
{
  if (!heap)
    return 0;
  size_t lchild_index = index * 2 + 1,
         rchild_index = index * 2 + 2;

  size_t smallest_index = index;

  if (lchild_index < heap->size && heap->items[lchild_index].val < heap->items[smallest_index].val)
    smallest_index = lchild_index;
  if (rchild_index < heap->size && heap->items[rchild_index].val < heap->items[smallest_index].val)
    smallest_index = rchild_index;

  if (smallest_index == index)
    return 1;

  HeapItem tmp = heap->items[index];
  heap->items[index] = heap->items[smallest_index];
  heap->items[smallest_index] = tmp;
  return sift_down(heap, smallest_index);
}

int Heap_add(Heap *heap, HeapItem item)
{
  if (!heap || !adjust_heap_capacity(heap))
    return 0;

  heap->items[heap->size] = item;
  return sift_up(heap, heap->size++);
}
HeapItem Heap_pop(Heap *heap)
{
  if (Heap_is_empty(heap))
    return (HeapItem){};

  HeapItem result = Heap_peak(heap);

  heap->items[0] = heap->items[--heap->size];
  sift_down(heap, 0);

  adjust_heap_capacity(heap);
  
  return result;
}

HeapItem Heap_peak(Heap *heap)
{
  if (Heap_is_empty(heap))
    return (HeapItem){};

  return heap->items[0];
}

int Heap_is_empty(Heap *heap)
{
  if (!heap)
    return 1;
  return heap->size == 0;
}
