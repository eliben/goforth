#include <assert.h>
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

  show_state(state, 0, state->here);


  // Main interpreter loop
  // TODO: does not support compilation yet
  while(1) {
    char word[256];
    size_t len = get_word(state->input, word, sizeof(word));
    if (len == 0) {
      // EOF reached
      break;
    }

    // Find the word in the dictionary.
    push_word(state, word, len);
  }

  return 0;
}

