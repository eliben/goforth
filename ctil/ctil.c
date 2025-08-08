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

  // TODO: the easiest way to load another file would be at runtime,
  // add a flag e.g. -i <filename> to the command line. 
  // Change `interpret` to accept a custom stream; call it once with the
  // opened file, and then with stdin.
  interpret(state);
  return 0;
}
