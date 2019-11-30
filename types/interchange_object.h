#ifndef __INTERCHANGE_OBJECT_H__
#define __INTERCHANGE_OBJECT_H__

#include <stdint.h>
#include "klv/klv.h"
#include "klv/batch.h"

typedef struct {
    /* UID */
    uint8_t instance_uid[KLV_KEY_SIZE];
    /* UID */
    uint8_t generation_uid[KLV_KEY_SIZE];
    /* AUID */
    uint8_t object_class[KLV_KEY_SIZE];
    /* Application Plugin Batch - Strong References */
    batch_t application_plugins;
} mxf_interchange_object_t;

void mxf_interchange_object_init(mxf_interchange_object_t *io);

void mxf_interchange_object_destroy(mxf_interchange_object_t *io);

void mxf_interchange_object_dump(mxf_interchange_object_t *io);

#endif