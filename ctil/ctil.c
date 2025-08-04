#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "die.h"
#include "input.h"
#include "state.h"
#include "zmalloc.h"


int main() {
  state_t* state = create_state();
  register_builtins(state);

  // Main interpreter loop
  // TODO: does not support compilation yet
  while (1) {
    char word[256];
    size_t len = get_word(state->input, word, sizeof(word));
    if (len == 0) {
      // EOF reached
      break;
    }

    int64_t entry_offset = find_word_in_dict(state, word);
    if (entry_offset != -1) {
      execute_word(state, entry_offset);
    } else {
      // Try to parse the word as a number.
      char* endptr;
      int64_t num = strtoll(word, &endptr, 10);
      if (*endptr == '\0') {
        // Successfully parsed a number, push it onto the stack.
        state->stacktop++;
        state->stack[state->stacktop] = num;
      } else {
        die("Unknown word: %s\n", word);
      }
    }
  }

  return 0;
}
