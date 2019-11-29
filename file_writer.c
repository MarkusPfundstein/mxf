#include "file_writer.h"
#include <stdio.h>

int
file_writer_open(file_writer_t *fw, const char *out_file)
{
    fw->fp = fopen(out_file, "w");
    if (fw->fp == NULL) {
        return 1;
    }
    return 0;
}

bool
file_writer_is_open(file_writer_t *fw) 
{
    return fw->fp != NULL;
}

int
file_writer_write(file_writer_t *fw, const char *str, uint64_t size)
{
    int rc;
    rc = fwrite(str, 1, size, fw->fp);
    if (rc != size) {
        return 1;
    }
    return 0;
}

int
file_writer_close(file_writer_t *fw)
{
    if (fw->fp != NULL) {
        fclose(fw->fp);
        fw->fp = NULL;
        return 0;
    }
    return 1;
}