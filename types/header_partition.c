#include "header_partition.h"
#include <string.h>
#include "utils/dumpers.h"

void mxf_header_partition_init(mxf_header_partition_t *hp)
{
    bzero(hp, sizeof(mxf_header_partition_t));
    batch_init(&hp->essence_containers);
}

void mxf_header_partition_destroy(mxf_header_partition_t *hp)
{
    batch_delete(&hp->essence_containers, NULL);
}

void mxf_header_partition_dump(mxf_header_partition_t *header_partition)
{
    printf("header_partition\r\n");
    printf("major_version:            %d\n", header_partition->major_version);
    printf("minor_version:            %d\n", header_partition->minor_version);
    printf("kag_size:                 %d\n", header_partition->kag_size);
    printf("this_partition:           %ld\n", header_partition->this_partition);
    printf("previous_partition:       %ld\n", header_partition->previous_partition);
    printf("footer_partition:         %ld\n", header_partition->footer_partition);
    printf("header_byte_count:        %ld\n", header_partition->header_byte_count);
    printf("index_byte_count:         %ld\n", header_partition->index_byte_count);
    printf("index_sid:                %d\n", header_partition->index_sid);
    printf("body_offset:              %ld\n", header_partition->body_offset);
    printf("body_sid:                 %d\n", header_partition->body_sid);
    dump_auid("operational_pattern:       ", header_partition->operation_pattern_u.bytes);
    dump_batch("essence_containers:       ", &header_partition->essence_containers);
}