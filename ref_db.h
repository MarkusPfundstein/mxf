#ifndef __REF_DB_H__
#define __REF_DB_H__

#include <stdint.h>
#include <stdbool.h>
#include "linked_list.h"

typedef void (*ref_db_entry_free_fn)(void *);

typedef struct {
    uint8_t uuid[16];
    void *entry;
    ref_db_entry_free_fn free_fn;
} ref_db_entry_t;

typedef struct {
    linked_list_t *ll;
} ref_db_t;

int ref_db_init(ref_db_t *rdb);
void ref_db_print(ref_db_t *rdb);
void *ref_db_find(ref_db_t *rdb, uint8_t uuid[16]);
int ref_db_add(ref_db_t *rdb, uint8_t uuid[16], void *data, ref_db_entry_free_fn free_fn);
void ref_db_destroy(ref_db_t *rdb);

#endif
