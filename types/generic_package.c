#include "generic_package.h"
#include <string.h>
#include "utils/dumpers.h"

void mxf_generic_package_init(mxf_generic_package_t *gp)
{
    bzero(gp, sizeof(mxf_generic_package_t));
    mxf_interchange_object_init(&gp->interchange_object);
    batch_init(&gp->tracks);
}

void mxf_generic_package_destroy(mxf_generic_package_t *gp)
{
    printf("destroy generic package\n");
    mxf_interchange_object_destroy(&gp->interchange_object);
    batch_delete(&gp->tracks, NULL);
}

void mxf_generic_package_dump(mxf_generic_package_t *gp)
{
    mxf_interchange_object_dump(&gp->interchange_object);
    dump_batch("tracks:               ", &gp->tracks);
}