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
  s->pc = 0;
  s->retstacktop = -1;
  s->compiling = 0;
  s->output = stdout;
  return s;
}

void push_data_stack(state_t* s, int64_t value) {
  assert(s->stacktop < (int64_t)(sizeof(s->stack) / sizeof(s->stack[0])) - 1);
  s->stacktop++;
  s->stack[s->stacktop] = value;
}

int64_t pop_data_stack(state_t* s) {
  assert(s->stacktop >= 0);
  return s->stack[s->stacktop--];
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

int entry_is_immediate(state_t* s, int64_t entry_offset) {
  return (s->mem[entry_offset + 8] & F_IMMEDIATE) != 0;
}

builtin_func_t entry_get_builtin_func(state_t* s, int64_t entry_offset) {
  int64_t name_len = s->mem[entry_offset + 9];
  int64_t addr_offset = entry_offset + 10 + name_len;
  builtin_func_t func;
  memcpy(&func, &s->mem[addr_offset], sizeof(func));
  return func;
}

int64_t entry_get_code_offset(state_t* s, int64_t entry_offset) {
  int64_t name_len = s->mem[entry_offset + 9];
  return entry_offset + 10 + name_len;
}

void execute_word(state_t* s, int64_t entry_offset) {
  if (entry_is_builtin(s, entry_offset)) {
    builtin_func_t func = entry_get_builtin_func(s, entry_offset);
    func(s);
    return;
  }

  // The entry is not a builtin; set pc to the first word of its code and
  // start executing.
  s->pc = entry_get_code_offset(s, entry_offset);
  while (1) {
    int64_t subentry = *(int64_t*)&s->mem[s->pc];
    if (subentry == -1) {
      // TODO: if this function can be called recursively, we may need
      // to adjust the check for "same level of retstack" here?
      if (s->retstacktop < 0) {
        break;
      }
      s->pc = s->retstack[s->retstacktop--];
    } else if (entry_is_builtin(s, subentry)) {
      // It's a builtin, execute it.
      builtin_func_t func = entry_get_builtin_func(s, subentry);
      func(s);
      s->pc += sizeof(int64_t);
    } else {
      s->retstacktop++;
      s->retstack[s->retstacktop] = s->pc + sizeof(int64_t);
      s->pc = entry_get_code_offset(s, subentry);
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
      if (entry_is_immediate(s, entry_offset) || !s->compiling) {
        // Execute directly.
        execute_word(s, entry_offset);
      } else {
        // Store the entry offset at the 'here' offset in memory.
        memcpy(&s->mem[s->here], &entry_offset, sizeof(int64_t));
        s->here += sizeof(int64_t);
      }
    } else {
      // Try to parse the word as a number.
      char* endptr;
      int64_t num = strtoll(word, &endptr, 10);
      if (*endptr != '\0') {
        die("Unknown word: %s\n", word);
      }
      // Successfully parsed a number.
      if (s->compiling) {
        // Store the entry for LITNUMBER following the number itself in memory.
        int64_t litnumber_offset = find_word_in_dict(s, "LITNUMBER");
        assert(litnumber_offset != -1);
        memcpy(&s->mem[s->here], &litnumber_offset, sizeof(int64_t));
        s->here += sizeof(int64_t);

        memcpy(&s->mem[s->here], &num, sizeof(int64_t));
        s->here += sizeof(int64_t);
      } else {
        push_data_stack(s, num);
      }
    }
  }
}