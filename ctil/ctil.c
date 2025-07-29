#include <stdio.h>
#include <stdlib.h>

#include "zmalloc.h"

typedef struct {
  char mem[64 * 1024];
  int pc;
} state_t;

state_t* create_state() {
  state_t* s = (state_t*)zmalloc(sizeof(state_t));
  s->pc = 0;
  return s;
}

int main() {
  printf("Hello, World!\n");

  return 0;
}