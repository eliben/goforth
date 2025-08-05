#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "state.h"

int main() {
  state_t* state = create_state();
  register_builtins(state);
  //   show_state(state, 0, state->here);
  interpret(state);
  return 0;
}
