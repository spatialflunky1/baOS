#ifndef STRING_H
#define STRING_H

#include <datatypes.h>

typedef int word; // "optimal size of data transfer for performance"

void* memcpy(void* sourcep, void* destp, uint32_t nbytes);
void* memset(void *destp, int constant, uint32_t nbytes);

#endif
