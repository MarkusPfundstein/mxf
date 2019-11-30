#include <string.h>
#include "material_package.h"
#include "utils/dumpers.h"

void mxf_material_package_init(mxf_material_package_t *mp)
{
    bzero(mp, sizeof(mxf_material_package_t));
    mxf_generic_package_init(&mp->generic_package);
}

void mxf_materal_package_destroy(mxf_material_package_t *mp)
{
    printf("destroy material package\n");
    mxf_generic_package_destroy(&mp->generic_package);
}

void mxf_material_package_dump(mxf_material_package_t *mp)
{
    mxf_generic_package_dump(&mp->generic_package);
    dump_auid("package_marker:        ", mp->package_marker);
}
