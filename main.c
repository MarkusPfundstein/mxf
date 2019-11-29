#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "file_writer.h"
#include "linked_list.h"
#include "klv.h"
#include "batch.h"
#include "ul.h"
#include "ref_db.h"

void dump_auid(const char* prefix, uint8_t *bytes)
{
    printf("%s", prefix);
    for (int i = 0; i < KLV_KEY_SIZE; ++i)
    {
        printf("%02x", *(bytes + i));
        if (i < KLV_KEY_SIZE - 1)
        {
            printf(".");
        }
    }
    printf("\n");
}


void dump_batch(const char *prefix, batch_t *b)
{
    printf("%s\n", prefix);
    printf("- count: %d\n", b->count);
    printf("- len:   %d\n", b->length);

    for (int i = 0; i < b->count; ++i) {
        uint8_t *tmp = (uint8_t *)ll_get_at_index(b->items, i);
        if (!tmp) {
            fprintf(stderr, "error with LL\n");
            continue;
        }
        dump_auid("item:         ", tmp);
    }
}
/* EOF batch.h */

/* klv.h defs */
typedef enum {
    MXF_KLV_PACK_KIND_HEADER = 0x2,
    MXF_KLV_PACK_KIND_BODY   = 0x3,
    MXF_KLV_PACK_KIND_FOOTER = 0x4,
    MXF_KLV_PACK_KIND_PRIMER = 0x5,
    MXF_KLV_PACK_KIND_RANDOM_INDEX = 0x11
} mxf_klv_pack_kind_t;

typedef enum {
    MXF_KLV_PARTITION_STATUS_OPEN_INCOMPLETE = 0x1,
    MXF_KLV_PARTITION_STATUS_CLOSED_INCOMPLETE = 0x2,
    MXF_KLV_PARTITION_STATUS_OPEN_COMPLETE = 0x3,
    MXF_KLV_PARTITION_STATUS_CLOSED_COMPLETE = 0x4
} mxf_klv_partition_status_t;

typedef struct {
    uint16_t local_tag;
    uint8_t uid[16];
} local_tag_entry_t;

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

typedef struct {
    mxf_interchange_object_t interchange_object;
    uint8_t package_uid[32];
    // name
    // package_creation_date
    // package_modified_date
    batch_t tracks;
} mxf_generic_package_t;

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
#if 0
    printf("last_modified_date:  %lu\n", preface->last_modified_date);
    printf("version:              %u\n", preface->version_type);
    printf("object_model_version: %u\n", preface->object_model_version);
    dump_auid("primary_package:       ", preface->primary_package);
    dump_auid("content_storage:       ", preface->content_storage);
    dump_auid("operational_pattern:   ", preface->operation_pattern_u.bytes);
    dump_batch("essence_containers:   ", &preface->essence_containers);
    dump_batch("dm_schemes:           ", &preface->dm_schemes);
#endif
}

typedef struct {
    mxf_generic_package_t generic_package;
    uint8_t package_marker[KLV_KEY_SIZE];
} mxf_material_package_t;

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

typedef struct {
    batch_t local_tag_entries;
    file_writer_t *out_stream;
    ref_db_t ref_db;
} mxf_context_t;

void mxf_context_init(mxf_context_t *mxf)
{
    batch_init(&mxf->local_tag_entries);
    ref_db_init(&mxf->ref_db);

    mxf->out_stream->fp = NULL;
}

void mxf_context_destroy(mxf_context_t *mxf)
{
    batch_delete(&mxf->local_tag_entries, NULL);

    if (mxf->out_stream->fp != NULL) {
        file_writer_close(mxf->out_stream);
    }

    ref_db_destroy(&mxf->ref_db);
}

void mxf_header_partition_init(mxf_header_partition_t *hp)
{
    bzero(hp, sizeof(mxf_header_partition_t));
    batch_init(&hp->essence_containers);
}

void mxf_header_partition_destroy(mxf_header_partition_t *hp)
{
    batch_delete(&hp->essence_containers, NULL);
}

int mxf_header_partition_dump(mxf_header_partition_t *header_partition)
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

bool mxf_resolve_local_tag_entry(batch_t *tags, uint16_t key, uint8_t out[KLV_KEY_SIZE])
{
    for (int i = 0; i < tags->count; ++i) {
        local_tag_entry_t *t = ll_get_at_index(tags->items, i);
        if (!t) {
            continue;
        }
        if (t->local_tag == key) {
            memcpy(out, t->uid, KLV_KEY_SIZE);
        }
    }

    return false;
}

int mxf_parse_klv_length_field_two_bytes(klv_parser_t *parser, klv_t *klv)
{
    fprintf(stderr, "2bytes niy");
    return 1;
}

bool mxf_test_ul_is_structural_metadata(klv_t *klv)
{
        return (
            klv->key.layout.category_designator == KLV_CAT_DES_GROUPS &&
            (klv->key.layout.registry_designator & 0x03) &&
            klv->key.layout.structure_designator == KLV_STRUCT_DES_GROUPS_SET_PACK_REGISTRY
        );
}

bool mxf_test_ul_is_structural_metadata_instance(klv_t *klv, uint8_t b1, uint8_t b2)
{
    return (
        mxf_test_ul_is_structural_metadata(klv) &&
        klv->key.layout.item_designator_u.structural_metadata.set_kind_1 == b1 &&
        klv->key.layout.item_designator_u.structural_metadata.set_kind_2 == b2 
    );
}

bool mxf_test_ul_is_primer_pack(klv_t *klv)
{
    return (
        klv->key.layout.category_designator == KLV_CAT_DES_GROUPS &&
        klv->key.layout.registry_designator == KLV_REG_DES_GROUPS_DEFINED_LENGTH_PACKS &&
        klv->key.layout.structure_designator == KLV_STRUCT_DES_GROUPS_SET_PACK_REGISTRY &&
        klv->key.layout.item_designator_u.partition_pack.set_pack_kind == MXF_KLV_PACK_KIND_PRIMER
    );
}

bool mxf_test_ul_is_partition(klv_t *klv)
{
    return (
        klv->key.layout.category_designator == KLV_CAT_DES_GROUPS &&
        klv->key.layout.registry_designator == KLV_REG_DES_GROUPS_DEFINED_LENGTH_PACKS &&
        klv->key.layout.structure_designator == KLV_STRUCT_DES_GROUPS_SET_PACK_REGISTRY && (
            klv->key.layout.item_designator_u.partition_pack.set_pack_kind == MXF_KLV_PACK_KIND_HEADER ||
            klv->key.layout.item_designator_u.partition_pack.set_pack_kind == MXF_KLV_PACK_KIND_FOOTER ||
            klv->key.layout.item_designator_u.partition_pack.set_pack_kind == MXF_KLV_PACK_KIND_BODY
        )
    );
}

int mxf_generic_package_read_track_ul(klv_parser_t *parser, uint32_t size, void **storage)
{
    int rc;
    uint8_t *temp;
    
    temp = malloc(size);

    rc = klv_read_bytes(parser, size, temp);

    *storage = temp;

    return rc;
}

int mxf_header_partition_read_essence_stream_ul(klv_parser_t *parser, uint32_t size, void **storage)
{
    int rc;
    uint8_t *temp;
    
    temp = malloc(size);

    rc = klv_read_bytes(parser, size, temp);

    *storage = temp;

    return rc;
}

void mxf_dump_local_tag_entries(batch_t *local_tag_entries) 
{
    for (int i = 0; i < local_tag_entries->count; ++i) {
        local_tag_entry_t *tmp = (local_tag_entry_t*)ll_get_at_index(local_tag_entries->items, i);
        if (!tmp) {
            fprintf(stderr, "error with LL\n");
            break;
        }
        printf("tag entry:        ");
        printf("0x%04x -> ", tmp->local_tag);
        for (int i = 0; i < KLV_KEY_SIZE; ++i)
        {
            printf("%02x", *(tmp->uid + i));
            if (i < KLV_KEY_SIZE - 1)
            {
                printf(".");
            }
        }
        printf("\n");
    }
}


int mxf_read_local_tag_entry(klv_parser_t *parser, uint32_t size, void **storage)
{
    local_tag_entry_t *local_tag_entry;
    int rc = 0;

    if (size != sizeof(local_tag_entry_t)) {
        fprintf(stderr, "Error sizeof local_tag_entry\n");
        return 1;
    }

    local_tag_entry = (local_tag_entry_t *)malloc(sizeof(local_tag_entry_t));
    
    rc |= klv_read_uint16(parser, &local_tag_entry->local_tag);
    rc |= klv_read_bytes(parser, 16, local_tag_entry->uid);

    *storage = local_tag_entry;

    return rc;
}

int mxf_read_local_tag_entries(klv_parser_t *parser, batch_t *local_tag_entries)
{
    return klv_read_batch(parser, local_tag_entries, mxf_read_local_tag_entry);
}

int mxf_read_application_plugin(klv_parser_t *parser, uint32_t size, void **storage)
{
    int rc;
    uint8_t *temp;

    printf("read app plugin: %d\n", size);
    
    temp = malloc(size);

    rc = klv_read_bytes(parser, size, temp);

    *storage = temp;

    return rc;
}

int mxf_read_header_partition(klv_parser_t *parser, mxf_header_partition_t *header_partition)
{
    int rc = 0;

    rc = klv_read_uint16(parser, &header_partition->major_version);
    rc |= klv_read_uint16(parser, &header_partition->minor_version);
    rc |= klv_read_uint32(parser, &header_partition->kag_size);
    rc |= klv_read_uint64(parser, &header_partition->this_partition);
    rc |= klv_read_uint64(parser, &header_partition->previous_partition);
    rc |= klv_read_uint64(parser, &header_partition->footer_partition);
    rc |= klv_read_uint64(parser, &header_partition->header_byte_count);
    rc |= klv_read_uint64(parser, &header_partition->index_byte_count);
    rc |= klv_read_uint32(parser, &header_partition->index_sid);
    rc |= klv_read_uint64(parser, &header_partition->body_offset);
    rc |= klv_read_uint32(parser, &header_partition->body_sid);
    rc |= klv_read_ul_raw(parser, header_partition->operation_pattern_u.bytes);
    rc |= klv_read_batch(parser, &header_partition->essence_containers, mxf_header_partition_read_essence_stream_ul);

    return rc;
}

bool mxf_test_key_is_valid_st377(klv_t *klv)
{
    /* do some sanity checking */
    return (
        klv->key.layout.oid == 0x06     &&
        klv->key.layout.ul_size == 0x0e &&
        klv->key.layout.ul_code == 0x2b &&
        klv->key.layout.smpte_designator == 0x34
    );
}


bool mxf_try_read_interchange_data(klv_parser_t *parser, uint8_t entry_ul[KLV_KEY_SIZE], mxf_interchange_object_t *io)
{
    if (memcmp(entry_ul, MXF_UL_IO_GENERATION_UID, KLV_KEY_SIZE) == 0) {
        printf("read generation uid\n");
        klv_read_bytes(parser, KLV_KEY_SIZE, io->generation_uid);
        return true;
    }
    else if (memcmp(entry_ul, MXF_UL_IO_INSTANCE_UID, KLV_KEY_SIZE) == 0) {
        printf("read instance uid\n");
        klv_read_bytes(parser, KLV_KEY_SIZE, io->instance_uid);
        return true;
    }
    else if (memcmp(entry_ul, MXF_UL_IO_OBJECT_CLASS, KLV_KEY_SIZE) == 0) {
        printf("read object class\n");
        klv_read_bytes(parser, KLV_KEY_SIZE, io->object_class);
        return true;
    } 
    return false;
}

bool mxf_try_read_generic_package(klv_parser_t *parser, uint8_t entry_ul[KLV_KEY_SIZE], mxf_generic_package_t *gp)
{
    if (mxf_try_read_interchange_data(parser, entry_ul, &gp->interchange_object)) {
        return true;
    }
    else if (memcmp(entry_ul, MXF_UL_GENERIC_PACKAGE_TRACKS, KLV_KEY_SIZE) == 0) {
        printf("read tracks\n");
        klv_read_batch(parser, &gp->tracks, mxf_generic_package_read_track_ul);
        return true;
    }
    return false;
}

int mxf_read_preface(klv_parser_t *parser, uint32_t klv_size, mxf_preface_t *preface)
{
    mxf_context_t *mxf;
    int rc = 0;
    uint16_t local_tag;
    uint16_t size;
    uint32_t n_read;        /* how many bytes have we read? */
    uint8_t entry_ul[KLV_KEY_SIZE];

    mxf = (mxf_context_t *)parser->context;

    n_read = 0;
    do {
        klv_read_uint16(parser, &local_tag);
        klv_read_uint16(parser, &size);
        mxf_resolve_local_tag_entry(&mxf->local_tag_entries, local_tag, entry_ul);

        if (mxf_try_read_interchange_data(parser, entry_ul, &preface->interchange_object)) {
            (void)0;
        }
        else if (memcmp(entry_ul, MXF_UL_PREFACE_LAST_MODIFIED_DATE, KLV_KEY_SIZE) == 0) {
            printf("read last_modified date\n");
            klv_read_uint64(parser, &preface->last_modified_date);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_OBJECT_MODEL_VERSION, KLV_KEY_SIZE) == 0) {
            printf("read object model version\n");
            klv_read_uint32(parser, &preface->object_model_version);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_VERSION, KLV_KEY_SIZE) == 0) {
            printf("read version\n");
            klv_read_uint16(parser, &preface->version_type);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_PRIMARY_PACKAGE, KLV_KEY_SIZE) == 0) {
            printf("read primary_package\n");
            klv_read_bytes(parser, KLV_KEY_SIZE, preface->primary_package);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_IDENTIFICATIONS, KLV_KEY_SIZE) == 0) {
            printf("SKIP identificationspackage (%d)\n", size);
            klv_parser_skip(parser, size);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_CONTENT_STORAGE, KLV_KEY_SIZE) == 0) {
            printf("read content_storage\n");
            klv_read_bytes(parser, KLV_KEY_SIZE, preface->content_storage);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_OPERATIONAL_PATTERN, KLV_KEY_SIZE) == 0) {
            printf("read operational_pattern\n");
            klv_read_bytes(parser, KLV_KEY_SIZE, preface->operation_pattern_u.bytes);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_ESSENCE_CONTAINERS, KLV_KEY_SIZE) == 0) {
            printf("read essence_containers\n");
            klv_read_batch(parser, &preface->essence_containers, mxf_header_partition_read_essence_stream_ul);
        } 
        else if (memcmp(entry_ul, MXF_UL_PREFACE_DM_SCHEMES, KLV_KEY_SIZE) == 0) {
            printf("read dm_schemes\n");
            klv_read_batch(parser, &preface->dm_schemes, mxf_header_partition_read_essence_stream_ul);
        } 
        else {
            printf("skip tag: %04x\n", local_tag);
            dump_auid("umatched uuid: ", entry_ul);
            klv_parser_skip(parser, size);
        }

        // 2 bytes (local tag) + 2 bytes (size) + size bytes
        n_read += (2 + 2 + size);

        printf("n_read: %d (%d)\n", n_read, klv_size);

    } while (n_read < klv_size);

    return 0;
}

int mxf_read_material_package(klv_parser_t *parser, uint64_t klv_size, mxf_material_package_t *material_package)
{
    mxf_context_t *mxf;
    int rc = 0;
    uint16_t local_tag;
    uint16_t size;
    uint32_t n_read;        /* how many bytes have we read? */
    uint8_t entry_ul[KLV_KEY_SIZE];

    mxf = (mxf_context_t *)parser->context;

    n_read = 0;
    do {
        klv_read_uint16(parser, &local_tag);
        klv_read_uint16(parser, &size);
        mxf_resolve_local_tag_entry(&mxf->local_tag_entries, local_tag, entry_ul);

        if (mxf_try_read_generic_package(parser, entry_ul, &material_package->generic_package)) {
            (void)0;
        }
        else if (memcmp(entry_ul, MXF_UL_MATERIAL_PACKAGE_PACKAGE_MARKER, KLV_KEY_SIZE) == 0) {
            printf("read package marker\n");
            klv_read_bytes(parser, KLV_KEY_SIZE, material_package->package_marker);
        } 
        else {
            printf("skip tag %04x\n", local_tag);
            dump_auid("umatched uuid: ", entry_ul);
            klv_parser_skip(parser, size);
        }

        n_read += (2 + 2 + size);

        printf("n_read: %d (%lu)\n", n_read, klv_size);

    } while (n_read < klv_size);

    return 0;

    return rc;
}

int mxf_parse_partition_header(klv_parser_t *parser, uint64_t partition_size)
{
    int rc = 0;
    uint64_t n_read = 0;
    klv_t klv = { 0 };
    klv_length_type_t length_type;
    mxf_context_t *mxf;

    mxf = (mxf_context_t*)parser->context;

    while (rc == 0 && (n_read < partition_size)) {
        printf("current pos in partition: %lu\n", n_read);

        rc = klv_parser_parse_key(parser, &klv);
        if (!mxf_test_key_is_valid_st377(&klv)) {
            fprintf(stderr, "invalid klv\n");
            return 1;
        }
        length_type = klv_resolve_length_type_from_ul(&klv.key.layout);
        rc = klv_parser_parse_length(parser, &klv, length_type);

        if (mxf_test_ul_is_primer_pack(&klv)) {
            printf("---> PRIMER PACK\n");
            klv_dump_ul(&klv);

            rc = mxf_read_local_tag_entries(parser, &mxf->local_tag_entries);
            mxf_dump_local_tag_entries(&mxf->local_tag_entries);
        } 
        /* STRUCTURAL METADATA - st377 Table 17 p, 73 */
        else if (mxf_test_ul_is_structural_metadata_instance(&klv, 0x01, 0x2f)) {
            /* preface */
            printf("---> PREFACE\n");
            mxf_preface_t *preface = malloc(sizeof(mxf_preface_t));
            mxf_preface_init(preface);

            rc = mxf_read_preface(parser, klv.length, preface);
            mxf_preface_dump(preface);

            ref_db_add(&mxf->ref_db, preface->interchange_object.instance_uid, preface, (ref_db_entry_free_fn)mxf_preface_destroy);
        }  else if (mxf_test_ul_is_structural_metadata_instance(&klv, 0x01, 0x36 )) {
            /* material package */
            printf("---> MATERIAL_PACKAGE\n");
            //klv_parser_skip(parser, klv.length);
            mxf_material_package_t *mp = malloc(sizeof(mxf_material_package_t));

            rc = mxf_read_material_package(parser, klv.length, mp);
            mxf_material_package_dump(mp);

            ref_db_add(&mxf->ref_db, mp->generic_package.interchange_object.instance_uid, mp, (ref_db_entry_free_fn) mxf_materal_package_destroy);
        } else {
            dump_auid("---> SKIP I.O.: ", klv.key.bytes);
            klv_parser_skip(parser, klv.length);
        }

        printf("add %lu\n", klv.length + 16 + 4);
        printf("klv_length_type: %d\n", length_type);

        // + <UL> + 4 (size of K+L)
        n_read += (klv.length + 16 + 4);
        printf("PARTITION READ (%lu/%lu)\n", n_read, partition_size);
    }

    if (n_read > partition_size) {
        printf("read too far... err\n");
        return 1;
    }

    printf("DONE READING PARTITION\n");

    return rc;

}

int mxf_parse_essence(klv_parser_t *parser)
{
    printf("PARSE ESSENCE\n");
    mxf_context_t *mxf = (mxf_context_t *)parser->context;
    int rc = 0;
    klv_t klv = { 0 };
    klv_length_type_t length_type;
    uint32_t track_number;
    uint8_t *tmp_data;

    rc = klv_parser_parse_key(parser, &klv);
    if (rc) {
        fprintf(stderr, "error parsing key\n");
        return rc;
    }
    if (!mxf_test_key_is_valid_st377(&klv)) {
        fprintf(stderr, "invalid klv\n");
        return 1;
    }
    length_type = klv_resolve_length_type_from_ul(&klv.key.layout);
    rc = klv_parser_parse_length(parser, &klv, length_type);
    if (rc) {
        fprintf(stderr, "error parsing length\n");
        return rc;
    }

    dump_auid("essence key: ", klv.key.bytes);
    printf("essence length: %lu\n", klv.length);

    track_number =  (klv.key.bytes[12] << 24) |
                    (klv.key.bytes[13] << 16) | 
                    (klv.key.bytes[14] << 8)  |
                    (klv.key.bytes[15]);

    printf("track number for essence: %u\n", track_number);
     
    if (track_number == 369167617) {

        if (file_writer_is_open(mxf->out_stream)) {
            tmp_data = malloc(klv.length);
            if (!tmp_data) {
                fprintf(stderr, "error allocating data for essence stream\n");
                return 1;
            }
            
            rc = klv_read_bytes(parser, klv.length, tmp_data);
            if (rc) {
                free(tmp_data);
                return rc;
            }

            rc = file_writer_write(mxf->out_stream, tmp_data, klv.length);
            if (rc) {
                free(tmp_data);
                return rc;
            }

            free(tmp_data);
        } else {
            printf("skip essence data\n");
            rc = klv_parser_skip(parser, klv.length);
            if (rc) {
                return rc;
            }
        }

    }

    return rc;
}

int mxf_parse_klv_value_field(klv_parser_t *parser, klv_t *klv)
{
    int rc = 0;
    mxf_context_t *mxf;

    mxf = (mxf_context_t*)parser->context;

    if (!mxf_test_key_is_valid_st377(klv)) {
        fprintf(stderr, "invalid klv\n");
        return 1;
    }

    if (mxf_test_ul_is_partition(klv)) {
        printf("PARTITION\n");
        klv_dump_ul(klv);
        mxf_header_partition_t header_partition;
        mxf_header_partition_init(&header_partition);

        rc = mxf_read_header_partition(parser, &header_partition);
        if (rc) {
            fprintf(stderr, "error reading header partition\n");
            return rc;
        }

        mxf_header_partition_dump(&header_partition);

        switch (klv->key.layout.item_designator_u.partition_pack.set_pack_kind) {
            case MXF_KLV_PACK_KIND_FOOTER:

                printf("FOOTER %ld\n", klv->length);
                //klv_parser_skip(parser, klv->length);
                break;
            case MXF_KLV_PACK_KIND_BODY:
                printf("BODY %ld\n", klv->length);
                rc = mxf_parse_essence(parser);
                break;
            case MXF_KLV_PACK_KIND_HEADER:
                printf("HEADER (size: %ld)\n", header_partition.header_byte_count);
                rc = mxf_parse_partition_header(parser, header_partition.header_byte_count);
                break;
        }

        mxf_header_partition_destroy(&header_partition);

    } else {
        printf("SKIP [%lu]\n", klv->length);
        klv_parser_skip(parser, klv->length);
        return 1;
    }

    return rc;
}


int main(int argc, char **argv) 
{
    mxf_context_t mxf;
    klv_parser_t parser;
    klv_t klv;
    int rc = 0; 

    mxf_context_init(&mxf);

    if (argc < 2) {
        printf("usage <mxf_in> <essence_stream_out>\n");
        mxf_context_destroy(&mxf);
        return 1;
    }

    if (argc >= 3) {
        rc = file_writer_open(mxf.out_stream, argv[2]);
        if (rc) {
            goto done;
        }
    }

    rc = klv_parser_open(&parser, &mxf, argv[1]);
    if (rc) {
        fprintf(stderr, "Error opening %s\n", argv[1]);
        rc = 1;
        goto done;
    }

    do {
        rc = klv_parser_next_klv(&parser, &klv);
        if (rc != 0) {
            fprintf(stderr, "error klv_parser_next_klv\n");
            goto done;
        }
    } while (1);

done:
    ref_db_print(&mxf.ref_db);
    mxf_context_destroy(&mxf);
    klv_parser_close(&parser);

    return rc;
}