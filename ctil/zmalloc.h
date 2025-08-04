#ifndef ZMALLOC_H
#define ZMALLOC_H

// zmalloc - a simple wrapper for malloc and free that dies on failure.
// The pointer returned by zmalloc is guaranteed to be non-NULL.

#include <stdlib.h>

void* zmalloc(size_t size);
void zfree(void* ptr);


#endif // ZMALLOC_H
