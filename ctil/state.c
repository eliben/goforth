#include "state.h"

#include "zmalloc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

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

void execute_word(state_t* s, int64_t entry_offset) {
  char flags = s->mem[entry_offset + 8];

  if (flags & F_BUILTIN) {
    // Call the builtin function.
    int64_t name_len = s->mem[entry_offset + 9];
    int64_t addr_offset = entry_offset + 10 + name_len;

    builtin_func_t func;
    memcpy(&func, &s->mem[addr_offset], sizeof(func));
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
