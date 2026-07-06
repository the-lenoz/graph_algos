#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>


typedef struct
{
  int val;
  uint64_t node_id;
} HeapItem;

typedef struct _heap Heap;

Heap *Heap_init();
void Heap_destroy(Heap *heap);

int Heap_add(Heap *heap, HeapItem item);
HeapItem Heap_pop(Heap *heap);

HeapItem Heap_peak(Heap *heap);

int Heap_is_empty(Heap *heap);


#endif // HEAP_H
