#include <string.h>
#include <assert.h>
#include "klv.h"

extern int mxf_parse_klv_value_field(klv_parser_t *parser, klv_t *klv);

int klv_parser_open(klv_parser_t *parser, void *context, const char *file)
{
    parser->fp = fopen(file, "rb");
    if (!parser->fp) {
        return 1;
    }

    // to-do: should be done by checking file or via explicit parameter
    parser->codec_settings.application_protocol = KLV_APP_ST337;
    switch (parser->codec_settings.application_protocol) {
        case KLV_APP_ST337:
            parser->resolve_ul_fn = mxf_parse_klv_value_field;
            break;
        default:
            return 1;
    }

    parser->context = context;

    return 0;
}

int klv_parser_skip(klv_parser_t *parser, uint64_t bytes)
{
    return fseek(parser->fp, bytes, SEEK_CUR);
}

int klv_parser_close(klv_parser_t *parser)
{
    fclose(parser->fp);
    return 0;
}

int klv_parser_parse_key(klv_parser_t *parser, klv_t *klv)
{
    bzero(klv->key.bytes, KLV_KEY_SIZE);

    size_t bytes_read = fread(klv->key.bytes, 1, KLV_KEY_SIZE, parser->fp);
    if (bytes_read != KLV_KEY_SIZE) {
        CHECK_FILE_ERROR(parser->fp)
    }

    return 0;
}

int klv_parser_parse_length_ber(klv_parser_t *parser, klv_t *klv)
{
    size_t rc;
    uint8_t bytes[8] = { 0 };
    uint8_t n_bytes;

    klv->length = 0;

    // read first byte
    rc = fread(&bytes[0], 1, 1, parser->fp);
    if (rc != 1) {
        CHECK_FILE_ERROR(parser->fp);
    }

    if (bytes[0] & 0x80) {
        n_bytes = bytes[0] & 0x7f;
        klv->length_field_size = n_bytes + 1;
        if (n_bytes > 8) {
            fprintf(stderr, "discovered klv package with length: %u (max: 8)\n", n_bytes);
            return 2;
        }

        bytes[0] = 0;
        rc = fread(bytes, 1, n_bytes, parser->fp);
        if (rc != n_bytes) {
            CHECK_FILE_ERROR(parser->fp);
        }
        for (int i = 0; i < n_bytes; ++i) {
            klv->length |= (bytes[i] << (n_bytes - i - 1) * 8);
        }
    } else {
        klv->length = bytes[0] & 0x7f;
        klv->length_field_size = 1;
    }

    return 0;
}

int klv_parser_parse_length_2bytes(klv_parser_t *parser, klv_t *klv)
{
    size_t rc;
    uint8_t bytes[2] = { 0 };

    klv->length = 0;
    klv->length_field_size = 2;

    // read first byte
    rc = fread(&bytes[0], 1, 2, parser->fp);
    if (rc != 2) {
        CHECK_FILE_ERROR(parser->fp);
    }

    printf("b[0] = %02x\n", bytes[0]);
    printf("b[1] = %02x\n", bytes[1]);

    klv->length = bytes[1] << 8 | bytes[0];

    fprintf(stderr, "tried to parse 2 bytes, length: %ld\n", klv->length);
    assert(0);
    return 0;
}

int klv_parser_parse_length(klv_parser_t *parser, klv_t *klv, klv_length_type_t length_type) 
{
    switch (length_type) {
        case KLV_LENGTH_TYPE_BER:
            return klv_parser_parse_length_ber(parser, klv);
        case KLV_LENGTH_TYPE_TWO_BYTES:
            return klv_parser_parse_length_2bytes(parser, klv);
        default:
            fprintf(stderr, "invalid klv_length_type\n");
            return -1;
    }
}

int klv_parser_parse_value(klv_parser_t *parser, klv_t *klv)
{
    return parser->resolve_ul_fn(parser, klv);
}

int klv_dump_ul(klv_t *klv)
{
    printf("oid:                      %02x\n", klv->key.layout.oid);
    printf("ul_size:                  %02x\n", klv->key.layout.ul_size);
    printf("ul_code:                  %02x\n", klv->key.layout.ul_code);
    printf("smpte_designator:         %02x\n", klv->key.layout.smpte_designator);
    printf("category_designator:      %02x\n", klv->key.layout.category_designator);
    printf("registry_designator:      %02x\n", klv->key.layout.registry_designator);
    printf("structure_designator:     %02x\n", klv->key.layout.structure_designator);
    printf("version_number:           %02x\n", klv->key.layout.version_number);
    printf("item_designator:          ");
    for (int i = 0; i < 8; ++i) {
        printf("%02x", klv->key.layout.item_designator_u.bytes[i]);
        if (i < 7) {
            printf(".");
        }
    }
    printf("\n");

    return 0;
}

klv_length_type_t klv_resolve_length_type_from_ul(ul_t *ul)
{
    // packs & and sets
    /*
    if (ul->category_designator == KLV_CAT_DES_GROUPS) {
        switch (ul->registry_designator) {
            case KLV_REG_DES_GROUPS_DEFINED_LENGTH_PACKS:
            case KLV_REG_DES_GROUPS_VARIABLE_LENGTH_PACKS_BER:
                return KLV_LENGTH_TYPE_BER;

            case KLV_REG_DES_GROUPS_LOCAL_SETS_TWO_BYTES:
            case KLV_REG_DES_GROUPS_VARIABLE_LENGTH_PACKS_TWO_BYTES:
                return KLV_LENGTH_TYPE_TWO_BYTES;

            default:
                fprintf(stderr, "registry_designator value %02x not handled yet\n", ul->registry_designator);
                assert(0);
        }
    }*/

    // default
    return KLV_LENGTH_TYPE_BER;
}

int klv_parser_next_klv(klv_parser_t *parser, klv_t *klv)
{
    int rc;
    klv_length_type_t length_type;

    printf("parse key\n");
    rc = klv_parser_parse_key(parser, klv);
    if (rc) {
        fprintf(stderr, "error parse key\n");
        return rc;
    }
    for (int i = 0; i < KLV_KEY_SIZE; ++i)
    {
        printf("%02x", *(klv->key.bytes + i));
        if (i < KLV_KEY_SIZE - 1)
        {
            printf(".");
        }
    }
    printf("\n");

    length_type = klv_resolve_length_type_from_ul(&klv->key.layout);

    printf("parse length\n");
    rc = klv_parser_parse_length(parser, klv, length_type);
    if (rc) {
        fprintf(stderr, "error parse length\n");
        return rc;
    }

    printf("parse value\n");
    rc = klv_parser_parse_value(parser, klv);
    if (rc) {
        fprintf(stderr, "error parse value\n");
        return rc;
    }

    return rc;
}

int klv_read_uint8(klv_parser_t *parser, uint8_t *out)
{
    int rc;
    rc = fread(out, 1, 1, parser->fp);
    if (rc != 1) {
        CHECK_FILE_ERROR(parser->fp);
    }
    return 0;
}

int klv_read_uint16(klv_parser_t *parser, uint16_t *out)
{
    uint8_t buf[2];
    int rc;

    rc = fread(buf, 1, 2, parser->fp);
    if (rc != 2) {
        CHECK_FILE_ERROR(parser->fp);
    }

    *out = (buf[0] << 8) | buf[1];

    return 0;
}

int klv_read_uint32(klv_parser_t *parser, uint32_t *out)
{
    uint8_t buf[4];
    int rc;

    rc = fread(buf, 1, 4, parser->fp);
    if (rc != 4) {
        CHECK_FILE_ERROR(parser->fp);
    }

    *out = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

    return 0;
}

int klv_read_uint64(klv_parser_t *parser, uint64_t *out)
{
    uint8_t buf[8];
    int rc;

    rc = fread(buf, 1, 8, parser->fp);
    if (rc != 8) {
        CHECK_FILE_ERROR(parser->fp);
    }

    *out = ((uint64_t)buf[0] << 56) |
           ((uint64_t)buf[1] << 48) |
           ((uint64_t)buf[2] << 40) |
           ((uint64_t)buf[3] << 32) |
           (buf[4] << 24) |
           (buf[5] << 16) |
           (buf[6] << 8) |
           buf[7];

    return 0;
}

int klv_read_uint64_lsb(klv_parser_t *parser, uint64_t *out)
{
    uint8_t buf[8];
    int rc;

    rc = fread(buf, 1, 8, parser->fp);
    if (rc != 8) {
        CHECK_FILE_ERROR(parser->fp);
    }

    *out = ((uint64_t)buf[7] << 56) |
           ((uint64_t)buf[6] << 48) |
           ((uint64_t)buf[5] << 40) |
           ((uint64_t)buf[4] << 32) |
           (buf[3] << 24) |
           (buf[2] << 16) |
           (buf[1] << 8) |
           buf[0];

    return 0;
}

int klv_read_ul_raw(klv_parser_t *parser, uint8_t *out)
{
    return klv_read_bytes(parser, 16, out);
}

int klv_read_bytes(klv_parser_t *parser, uint32_t count, uint8_t *bytes)
{
    int rc;
    
    rc = fread(bytes, 1, count, parser->fp);
    if (rc != count) {
        CHECK_FILE_ERROR(parser->fp);
    }
    return 0;
}

int klv_read_batch(klv_parser_t *parser, batch_t *batch, klv_read_batch_fn on_value)
{
    int rc;
    void *storage;

    rc = klv_read_uint32(parser, &batch->count);
    rc |= klv_read_uint32(parser, &batch->length);
    for (int i = 0; i < batch->count; ++i) {
        rc |= on_value(parser, batch->length, &storage);
        batch->items = ll_append(batch->items, storage);
    }

    return rc;
}

uint16_t klv_get_full_local_tag_size(uint16_t size)
{
        // 2 bytes (local tag) + 2 bytes (size) + size bytes
        return (KLV_LOCAL_TAG_SIZE + KLV_LOCAL_TAG_LENGTH_FIELD_SIZE + size);
}

uint64_t klv_get_full_klv_size(klv_t *klv)
{
    return KLV_KEY_SIZE + klv->length + klv->length_field_size;
}