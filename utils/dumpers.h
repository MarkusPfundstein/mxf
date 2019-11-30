#ifndef __DUMPERS_H__
#define __DUMPERS_H__

#include <stdint.h>
#include "klv/batch.h"

// main.c
extern void dump_auid(const char* prefix, uint8_t *bytes);
extern void dump_batch(const char *prefix, batch_t *b);

#endif