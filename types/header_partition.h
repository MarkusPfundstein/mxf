#ifndef __HEADER_PARTITION_H__
#define __HEADER_PARTITION_H__

#include <stdint.h>
#include "klv/klv.h"
#include "klv/batch.h"

typedef struct {
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t kag_size;
    uint64_t this_partition;
    uint64_t previous_partition;
    uint64_t footer_partition;
    uint64_t header_byte_count;
    uint64_t index_byte_count;
    uint32_t index_sid;
    uint64_t body_offset;
    uint32_t body_sid;
    union {
        ul_t operational_pattern;
        uint8_t bytes[16];
    } operation_pattern_u;
    batch_t essence_containers;
} mxf_header_partition_t;

void mxf_header_partition_init(mxf_header_partition_t *hp);
void mxf_header_partition_destroy(mxf_header_partition_t *hp);
void mxf_header_partition_dump(mxf_header_partition_t *header_partition);

#endif
