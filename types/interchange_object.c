#include "interchange_object.h"
#include <string.h>
#include "utils/dumpers.h"

void mxf_interchange_object_init(mxf_interchange_object_t *io)
{
    bzero(io, sizeof(mxf_interchange_object_t));

    batch_init(&io->application_plugins);
}

void mxf_interchange_object_destroy(mxf_interchange_object_t *io)
{
    printf("destroy I.O.\n");
    batch_delete(&io->application_plugins, NULL);
}

void mxf_interchange_object_dump(mxf_interchange_object_t *io)
{
    dump_auid("instance_uid:          ", io->instance_uid);
    dump_auid("generation_uid:        " , io->generation_uid);
    dump_auid("object_class:          ", io->object_class);
}
