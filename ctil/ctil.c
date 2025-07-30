#include <stdio.h>
#include <stdlib.h>

#include "zmalloc.h"

typedef struct {
  char mem[64 * 1024];

  // Points to the link slot of the latest defined word.
  int latest;

  // Points to the place where the next word can be defined.
  int here;

  // Current program counter.
  int pc;
} state_t;

state_t* create_state() {
  state_t* s = (state_t*)zmalloc(sizeof(state_t));
  s->pc = 0;
  s->latest = 0;
  s->here = 0;
  return s;
}

int main() {
  printf("Hello, World!\n");

  return 0;
}