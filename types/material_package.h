#ifndef __MATERIAL_PACKAGE_H__
#define __MATERIAL_PACKAGE_H__

#include "generic_package.h"

typedef struct {
    mxf_generic_package_t generic_package;
    uint8_t package_marker[KLV_KEY_SIZE];
} mxf_material_package_t;

void mxf_material_package_init(mxf_material_package_t *mp);
void mxf_materal_package_destroy(mxf_material_package_t *mp);
void mxf_material_package_dump(mxf_material_package_t *mp);

#endif