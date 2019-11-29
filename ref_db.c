#include "ref_db.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int ref_db_init(ref_db_t *rdb)
{
    bzero(rdb, sizeof(ref_db_t));
    return 0;
}

void *ref_db_find(ref_db_t *rdb, uint8_t uuid[16])
{
    // to-do:
    // add functionality to linked list that just walks list without popping
    uint32_t len = ll_len(rdb->ll);
    for (int i = 0; i < len; ++i) {
        ref_db_entry_t *entry = ll_get_at_index(rdb->ll, i);
        if (entry) {
            if (memcmp(uuid, entry->uuid, 16) == 0) {
                return entry->entry;
            }
        } else {
            // shouldnt happen
            printf("linked list error\n");
            assert(0);
        }
    }
    return NULL;

}

int ref_db_add(ref_db_t *rdb, uint8_t uuid[16], void *data, ref_db_entry_free_fn free_fn)
{
    ref_db_entry_t *entry = malloc(sizeof(ref_db_entry_t));
    memcpy(entry->uuid, uuid, 16);
    entry->entry = data;
    entry->free_fn = free_fn;
    rdb->ll = ll_append(rdb->ll, entry);
}

void free_entry(void *ptr)
{
    ref_db_entry_t *entry = ptr;
    printf("delete entry from ref_db: %p\n", entry->free_fn);
    if (entry->free_fn != NULL) {
        printf("entry: %p\n", entry->free_fn);
        entry->free_fn(entry->entry);
    }
    free(entry);
}

void ref_db_destroy(ref_db_t *rdb)
{
    if (rdb->ll) {
        ll_free(rdb->ll, free_entry);
    }
}

void ref_db_print(ref_db_t *rdb)
{
    uint32_t len = ll_len(rdb->ll);
    printf("ref_db.length: %u\n", len);
    for (int i = 0; i < len; ++i) {
        ref_db_entry_t *entry = ll_get_at_index(rdb->ll, i);
        printf("entry %04d: ", (i+1));
        for (int i = 0; i < 16; ++i) {
            printf("%02x ", entry->uuid[i]);
        }
        printf("\n");
    }
}

#if 0
    rc = klv_read_uint32(parser, &batch->count);
    rc |= klv_read_uint32(parser, &batch->length);
    for (int i = 0; i < batch->count; ++i) {
        rc |= on_value(parser, batch->length, &storage);
        batch->items = ll_append(batch->items, storage);
    }
    #endif