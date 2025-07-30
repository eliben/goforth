#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "zmalloc.h"

typedef struct {
  char mem[64 * 1024];

  // Points to the link slot of the latest defined word.
  int64_t latest;

  // Points to the place where the next word can be defined.
  int64_t here;

  // Current program counter.
  int64_t pc;

  // Forth data stack.
  int64_t datastack[64 * 1024];
  
  // Pointer to the top item on the data stack.
  int64_t stacktop;

} state_t;

state_t* create_state() {
  state_t* s = (state_t*)zmalloc(sizeof(state_t));
  s->pc = 0;
  s->latest = 0;
  s->here = 0;
  s->stacktop = -1;
  return s;
}

int main() {
  printf("Hello, World!\n");
  state_t* state = create_state();



  return 0;
}