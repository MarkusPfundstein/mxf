#include "batch.h"
#include <string.h>

int batch_init(batch_t *batch)
{
    bzero(batch, sizeof(batch_t));
    batch->count = 0;
    batch->length = 0;
    return 0;
}

int batch_delete(batch_t *batch, free_user_data_func_t freefn)
{
    if (batch->count > 0) {
        ll_free(batch->items, freefn);
    }
    return 0;
}