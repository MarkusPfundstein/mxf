#ifndef __BATCH_H__
#define __BATCH_H__

#include <stdint.h>
#include "linked_list.h"

typedef struct {
    uint32_t count;
    uint32_t length;
    linked_list_t *items;
} batch_t;

int batch_init(batch_t *batch);
int batch_delete(batch_t *batch, free_user_data_func_t freefn);

#endif