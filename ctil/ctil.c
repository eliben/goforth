#include <stdio.h>
#include <stdlib.h>

#include "zmalloc.h"

typedef struct {
  char mem[64 * 1024];
  int pc;
} state_t;

state_t* create_state() {
  state_t* new_state = (state_t*)zmalloc(sizeof(state_t));
  new_state->pc = 0; // Initialize program counter
  return new_state;
}

int main() {
  // Example of a simple C program that prints "Hello, World!"
  printf("Hello, World!\n");

  // Return success status
  return 0;
}