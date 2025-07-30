#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "state.h"
#include "builtins.h"
#include "zmalloc.h"


int main() {
  printf("Hello, World!\n");
  state_t* state = create_state();
  register_builtins(state);

  return 0;
}

