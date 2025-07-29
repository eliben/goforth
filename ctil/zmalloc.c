#include <stdio.h>
#include <stdlib.h>

void die(const char* str) {
  fprintf(stderr, "Error: %s\n", str);
  exit(1);
}

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
