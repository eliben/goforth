#include <stdio.h>
#include <stdlib.h>
#include "die.h"

void* zmalloc(size_t size) {
  void* mem = malloc(size);
  if (!mem) {
    die("Memory allocation failed!");
  }
  return mem;
}

void zfree(void* ptr) {
  free(ptr);
}
