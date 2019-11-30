#ifndef __PREFACE_H__
#define __PREFACE_H__

#include "interchange_object.h"

typedef struct {
    mxf_interchange_object_t interchange_object;
    uint64_t last_modified_date;
    uint16_t version_type;
    uint32_t object_model_version;
    uint8_t  primary_package[KLV_KEY_SIZE];
    // array identifications
    uint8_t  content_storage[KLV_KEY_SIZE];
    union {
        ul_t operational_pattern;
        uint8_t bytes[16];
    } operation_pattern_u;
    batch_t essence_containers;
    batch_t dm_schemes;
    batch_t application_schemes;
} mxf_preface_t;

void mxf_preface_init(mxf_preface_t *preface);
void mxf_preface_destroy(mxf_preface_t *preface);
void mxf_preface_dump(mxf_preface_t *preface);

#endif