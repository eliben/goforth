#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "input.h"
#include "state.h"
#include "zmalloc.h"

// Find a word in the dictionary by its name. Returns the offset of the
// dictionary entry in mem if found, or -1 if not found.
int64_t find_word_in_dict(state_t* s, const char* word) {
  int64_t entry_offset = s->latest;

  while (entry_offset != -1) {
    char* entry_name = &s->mem[entry_offset + 10];
    if (strcmp(entry_name, word) == 0) {
      return entry_offset;
    }
    entry_offset = *(int64_t*)&s->mem[entry_offset];
  }

  return -1;
}

// Execute the word found at the given memory offset.
void execute_word(state_t* s, int64_t entry_offset) {
  // Execute the found word.
  char flags = s->mem[entry_offset + 8];
  if (flags & F_BUILTIN) {
    int64_t name_len = s->mem[entry_offset + 9];
    int64_t addr_offset = entry_offset + 10 + name_len;

    builtin_func_t func;
    memcpy(&func, &s->mem[addr_offset], sizeof(func));
    func(s);
  } else {
    assert(0 && "not builtin");
  }
}

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
        // TODO: have a proper die
        fprintf(stderr, "Unknown word: %s\n", word);
        exit(1);
      }
    }
  }

  return 0;
}
