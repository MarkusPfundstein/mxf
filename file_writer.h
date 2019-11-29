#ifndef __FILE_WRITER_H__
#define __FILE_WRITER_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    FILE *fp;
} file_writer_t;

extern int file_writer_open(file_writer_t *fw, const char *out_file);
extern bool file_writer_is_open(file_writer_t *fw);
extern int file_writer_write(file_writer_t *fw, const char *str, uint64_t size);
extern int file_writer_close(file_writer_t *fw);

#endif