#ifndef __GENERIC_PACKAGE_H__
#define __GENERIC_PACKAGE_H__

#include "interchange_object.h"

typedef struct {
    mxf_interchange_object_t interchange_object;
    uint8_t package_uid[32];
    // name
    // package_creation_date
    // package_modified_date
    batch_t tracks;
} mxf_generic_package_t;

void mxf_generic_package_init(mxf_generic_package_t *gp);
void mxf_generic_package_destroy(mxf_generic_package_t *gp);
void mxf_generic_package_dump(mxf_generic_package_t *gp);

#endif