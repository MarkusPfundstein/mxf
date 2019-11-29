#ifndef __KLV_H__
#define __KLV_H__

#include <stdint.h>
#include <stdio.h>
#include "batch.h"

#define KLV_KEY_SIZE    16

#define CHECK_FILE_ERROR(X) do{\
    if (feof(X) || ferror(X)) {\
        fprintf(stderr, "error reading file or EOF\r\n");\
        return -1;\
    }\
}while(0);

typedef enum {
    KLV_CAT_DES_DICT = 0x01,
    KLV_CAT_DES_GROUPS = 0x02,
    KLV_CAT_DES_WRAPPERS = 0x03,
    KLV_CAT_DES_LABELS = 0x04,
    KLV_CAT_DES_REGISTERED_PRIVATE = 0x05
} klv_category_designator_t;

typedef enum {
    KLV_REG_DES_DICT_METADATA = 0x1,
    KLV_REG_DES_DICT_ESSENCE = 0x2,
    KLV_REG_DES_DICT_CONTROL = 0x3,
    KLV_REG_DES_DICT_TYPES = 0x4
} klv_registry_designator_dictionary_t;

typedef enum {
    KLV_REG_DES_GROUPS_UNIVERSAL_SETS                   = 0x01,
    KLV_REG_DES_GROUPS_GLOBAL_SETS                      = 0x02,
    KLG_REG_DES_GROUPS_LOCAL_SETS                       = 0x03,
    //KLV_REG_DES_GROUPS_LOCAL_SETS_TWO_BYTES             = 0x53,
    KLV_REG_DES_GROUPS_VARIABLE_LENGTH_PACKS_BER        = 0x04,
    KLV_REG_DES_GROUPS_VARIABLE_LENGTH_PACKS_ONE_BYTE   = 0x24,
    KLV_REG_DES_GROUPS_VARIABLE_LENGTH_PACKS_TWO_BYTES  = 0x44,
    KLV_REG_DES_GROUPS_VARIABLE_LENGTH_PACKS_FOUR_BYTES = 0x64,
    KLV_REG_DES_GROUPS_DEFINED_LENGTH_PACKS             = 0x05
} klv_registry_designator_groups_t;

typedef enum {
    KLV_LENGTH_TYPE_BER        = 0x01,
    KLV_LENGTH_TYPE_ONE_BYTES,
    KLV_LENGTH_TYPE_TWO_BYTES,
    KLV_LENGTH_TYPE_FOUR_BYTES,
} klv_length_type_t;

typedef enum {
    KLV_STRUCT_DES_GROUPS_SET_PACK_REGISTRY = 0x1
} klv_structure_designator_groups_t;

typedef struct {
    uint8_t item_designator;
    uint8_t organization;
    uint8_t application;
    uint8_t structure_version;
    uint8_t structure_kind;
    uint8_t set_pack_kind;
    uint8_t partition_status;
    uint8_t reserved;
} ul_item_designator_partition_pack_t;

typedef struct {
    uint8_t item_designator;
    uint8_t organization;
    uint8_t application;
    uint8_t structure_version;
    uint8_t structure_kind;
    uint8_t set_kind_1;
    uint8_t set_kind_2;
    uint8_t reserved;
} ul_item_designator_structural_metadata_t;

typedef struct {
    /* --- st 298 ---
    /* Object Identifier - Always x06 */
    uint8_t oid;
    /* 16-byte size of UL - Always 0x0E */
    uint8_t ul_size;
    /* Concatenated sub-identifiers ISO, ORG - Always 0x2B */
    uint8_t ul_code;
    /* SMPTE Sub-Identifier - Always 0x34 */
    uint8_t smpte_designator;
    /* --- st 336 ---
    /* Category designator identifying the category of registry defined (e.g. Dictionaries) */
    uint8_t category_designator;
    /* Registry designator identifying the register in a catogry (e.g. Metadata dictionaries) */
    uint8_t registry_designator;
    /* Designator of the structure variant within the given registry designator */
    uint8_t structure_designator;
    /* Version of the given register which first defines the item specified by the Item Designator */
    uint8_t version_number;
    /* --- UL Sub-identifiers defined by application standards */
    /* Unique identification of the particular item */
    union {
        uint8_t bytes[8];
        ul_item_designator_partition_pack_t partition_pack;
        ul_item_designator_structural_metadata_t structural_metadata;
    } item_designator_u;
} ul_t;


/* klv - st336-2017 */
typedef struct klv_s {
    union { 
        ul_t layout;
        uint8_t bytes[KLV_KEY_SIZE];
    } key;
    /* variable length */
    uint64_t length;
} klv_t;

typedef enum {
    KLV_APP_ST337                       /* mxf */
} KLV_APPLICATION_PROTOCOL;

typedef struct {
    KLV_APPLICATION_PROTOCOL application_protocol;
} klv_codec_settings_t;

struct klv_parser_s;

typedef int (*klv_resolve_ul_fn)(struct klv_parser_s *parser, klv_t *klv);

typedef struct klv_parser_s {
    klv_codec_settings_t codec_settings;
    klv_resolve_ul_fn resolve_ul_fn;
    FILE *fp;
    void *context;
} klv_parser_t;

typedef int (*klv_read_batch_fn)(klv_parser_t* parser, uint32_t storage_size, void **storage);

int klv_dump_ul(klv_t *klv);
int klv_read_uint16(klv_parser_t *parser, uint16_t *out);
int klv_read_uint32(klv_parser_t *parser, uint32_t *out);
int klv_read_uint64(klv_parser_t *parser, uint64_t *out);
int klv_read_uint64_lsb(klv_parser_t *parser, uint64_t *out);
int klv_read_bytes(klv_parser_t *parser, uint32_t count, uint8_t *bytes);
int klv_read_ul_raw(klv_parser_t *parser, uint8_t *ul);
int klv_read_batch(klv_parser_t *parser, batch_t *batch, klv_read_batch_fn read_batch_fn);

int klv_parser_open(klv_parser_t *parser, void *, const char* filename);
int klv_parser_skip(klv_parser_t *parser, uint64_t bytes);
int klv_parser_next_klv(klv_parser_t *parser, klv_t *klv);
int klv_parser_close(klv_parser_t *parser);

klv_length_type_t klv_resolve_length_type_from_ul(ul_t *ul);
int klv_parser_parse_length(klv_parser_t *parser, klv_t *klv, klv_length_type_t length_type);
int klv_parser_parse_key(klv_parser_t *parser, klv_t *klv);

#endif