#ifndef HTABLE_D
#define HTABLE_D

#include <stddef.h>
#include <stdint.h>

typedef struct _hte _htelem;

typedef struct _htable HTable;

HTable *ht_init(size_t init_cap);
void ht_destroy(HTable *ht);

int ht_set(HTable *ht, const char *k, int64_t v);
int ht_get(HTable *ht, const char *k, int64_t *dest);
int ht_unset(HTable *ht, const char *k);

#endif // HTABLE_H
