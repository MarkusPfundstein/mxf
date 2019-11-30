#include "preface.h"
#include <string.h>
#include "utils/dumpers.h"

void mxf_preface_init(mxf_preface_t *preface)
{
    bzero(preface, sizeof(mxf_preface_t));
    mxf_interchange_object_init(&preface->interchange_object);
    batch_init(&preface->essence_containers);
    batch_init(&preface->dm_schemes);
    batch_init(&preface->application_schemes);
}

void mxf_preface_destroy(mxf_preface_t *preface)
{
    printf("destroy preface\n");

    mxf_interchange_object_destroy(&preface->interchange_object);
    batch_delete(&preface->essence_containers, NULL);
    batch_delete(&preface->dm_schemes, NULL);
    batch_delete(&preface->application_schemes, NULL);
}

void mxf_preface_dump(mxf_preface_t *preface)
{
    mxf_interchange_object_dump(&preface->interchange_object);
    printf("last_modified_date:   %lu\n", preface->last_modified_date);
    printf("version:               %u\n", preface->version_type);
    printf("object_model_version:  %u\n", preface->object_model_version);
    dump_auid("primary_package:       ", preface->primary_package);
    dump_auid("content_storage:       ", preface->content_storage);
    dump_auid("operational_pattern:   ", preface->operation_pattern_u.bytes);
    dump_batch("essence_containers:    ", &preface->essence_containers);
    dump_batch("dm_schemes:            ", &preface->dm_schemes);
    dump_batch("application_schemes:   ", &preface->application_schemes);
}
