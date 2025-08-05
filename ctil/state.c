#include "state.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "die.h"
#include "input.h"
#include "zmalloc.h"

state_t* create_state() {
  state_t* s = (state_t*)zmalloc(sizeof(state_t));
  memset(s->mem, 0, sizeof(s->mem));
  s->latest = -1;
  s->here = 0;
  s->stacktop = -1;
  s->retstacktop = -1;
  s->input = stdin;
  s->output = stdout;
  return s;
}

void show_state(state_t* s, uintptr_t start, uintptr_t len) {
  // Print latest, here etc.
  printf("State:\n");
  printf("  mem: %p\n", (void*)s->mem);
  printf("  latest: 0x%lx\n", (uint64_t)s->latest);
  printf("  here: 0x%lx\n", (uint64_t)s->here);

  // Print the memory region from start to start + len.
  printf("Memory dump from 0x%lx to 0x%lx:\n", start, start + len);
  for (uintptr_t i = start; i < start + len; i++) {
    if (i % 8 == 0) {
      if (i != start) {
        printf("\n");
      }
      printf("%08lx: ", i);
    }
    printf("%02x ", (unsigned char)s->mem[i]);
  }
  printf("\n");
}

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

int entry_is_builtin(state_t* s, int64_t entry_offset) {
  return (s->mem[entry_offset + 8] & F_BUILTIN) != 0;
}

builtin_func_t entry_get_builtin_func(state_t* s, int64_t entry_offset) {
  int64_t name_len = s->mem[entry_offset + 9];
  int64_t addr_offset = entry_offset + 10 + name_len;
  builtin_func_t func;
  memcpy(&func, &s->mem[addr_offset], sizeof(func));
  return func;
}

// TODO idea for real pc-based execution
//
// When called, assumes it's a non-builtin (top-level builtins can be
// special-cased).
//
// * pc starts pointing at the first word of the definition
// * while not "done"
//   * if word at pc is builtin, call it, advance pc to next word
//   * otherwise:
//      - save offset of _next_ word on the return stack
//      - set pc to the first word of the definition
//
// The last word of a definition is a special "exit" word that pops from
// the return stack to pc.

void execute_word(state_t* s, int64_t entry_offset) {
  //   char flags = s->mem[entry_offset + 8];

  if (entry_is_builtin(s, entry_offset)) {
    builtin_func_t func = entry_get_builtin_func(s, entry_offset);
    func(s);
  } else {
    int64_t name_len = s->mem[entry_offset + 9];
    int64_t addr_offset = entry_offset + 10 + name_len;
    while (1) {
      // Read the next word from the dictionary.
      int64_t next_word_offset = *(int64_t*)&s->mem[addr_offset];
      if (next_word_offset == -1) {
        // End of the word.
        break;
      }

      // Execute the next word.
      execute_word(s, next_word_offset);
      addr_offset += sizeof(int64_t);
    }
  }
}

void interpret(state_t* s) {
  while (1) {
    char word[256];
    size_t len = get_word(s->input, word, sizeof(word));
    if (len == 0) {
      // EOF reached
      break;
    }

    int64_t entry_offset = find_word_in_dict(s, word);
    if (entry_offset != -1) {
      execute_word(s, entry_offset);
    } else {
      // Try to parse the word as a number.
      char* endptr;
      int64_t num = strtoll(word, &endptr, 10);
      if (*endptr == '\0') {
        // Successfully parsed a number, push it onto the stack.
        s->stacktop++;
        s->stack[s->stacktop] = num;
      } else {
        die("Unknown word: %s\n", word);
      }
    }
  }
}