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
  s->loop_compile_stack_top = -1;
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

size_t push_new_loop_entry(state_t* s, int64_t start_offset) {
  assert(s->loop_compile_stack_top < MAX_NESTED_LOOPS - 1);
  s->loop_compile_stack_top++;
  s->loop_compile_stack[s->loop_compile_stack_top].backpatch_count = 0;
  s->loop_compile_stack[s->loop_compile_stack_top].start_offset = start_offset;
  return s->loop_compile_stack_top;
}

loop_compile_entry_t pop_loop_entry(state_t* s) {
  assert(s->loop_compile_stack_top >= 0);
  return s->loop_compile_stack[s->loop_compile_stack_top--];
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

void debug_dump_mem(state_t* s, uintptr_t start, uintptr_t len) {
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

void debug_dump_dict(state_t* s) {
  int64_t entry_offset = s->latest;

  // Dump all dictionary entries, followed the linked list of link offsets
  // in the header.
  while (entry_offset != -1) {
    char* entry_name = &s->mem[entry_offset + 10];

    printf("Entry at 0x%lx: name='%s'", entry_offset, entry_name);
    if (entry_is_immediate(s, entry_offset)) {
      printf(" (immediate)");
    }
    if (entry_is_builtin(s, entry_offset)) {
      printf(" (builtin)");
    }
    printf("\n");

    if (!entry_is_builtin(s, entry_offset)) {
      int64_t code_offset = entry_get_code_offset(s, entry_offset);

      // Show each word until we hit the end marker (-1). Words are typically
      // entries for other words, but can also be special words like
      // LITNUMBER and BRANCH.
      int64_t code_word = *(int64_t*)&s->mem[code_offset];
      while (code_word != -1) {
        printf("  %04lx:    %04lx ", code_offset, code_word);

        char* word_name = &s->mem[code_word + 10];
        printf("    %s", word_name);

        if (!strcmp(word_name, "LITNUMBER")) {
          code_offset += sizeof(int64_t);
          // LITNUMBER is special, it has a number following it.
          int64_t number = *(int64_t*)&s->mem[code_offset];
          printf(" %ld", number);
        } else if (!strcmp(word_name, "LITSTRING")) {
          code_offset += sizeof(int64_t);
          // LITSTRING has a string length and the string itself following it.
          int64_t str_len = *(int64_t*)&s->mem[code_offset];
          code_offset += sizeof(int64_t);
          char* str = &s->mem[code_offset];
          printf(" '%.*s'", (int)str_len, str);
          code_offset += str_len;
        } else if (!strcmp(word_name, "BRANCH") ||
                   !strcmp(word_name, "0BRANCH") ||
                   !strcmp(word_name, "_DOQIMPL") ||
                   !strcmp(word_name, "_LEAVEIMPL") ||
                   !strcmp(word_name, "_LOOPIMPL") ||
                   !strcmp(word_name, "_PLOOPIMPL")) {
          code_offset += sizeof(int64_t);
          int64_t branch_offset = *(int64_t*)&s->mem[code_offset];
          printf(" -> %ld (%04lx)", branch_offset, code_offset + branch_offset);
        }
        printf("\n");
        code_offset += sizeof(int64_t);
        code_word = *(int64_t*)&s->mem[code_offset];
      }
      printf("\n");
    }

    printf("\n");
    entry_offset = *(int64_t*)&s->mem[entry_offset];
  }
}

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

void place_dict_word(state_t* s, const char* word) {
  int64_t word_offset = find_word_in_dict(s, word);
  assert(word_offset != -1);
  memcpy(&s->mem[s->here], &word_offset, sizeof(int64_t));
  s->here += sizeof(int64_t);
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
      // Word is found in the dictionary.
      if (entry_is_immediate(s, entry_offset) || !s->compiling) {
        // Execute directly.
        execute_word(s, entry_offset);
      } else {
        // Compilation mode, and the word is not immediate.
        // Store the entry offset at the 'here' offset in memory.
        memcpy(&s->mem[s->here], &entry_offset, sizeof(int64_t));
        s->here += sizeof(int64_t);
      }
    } else {
      // Word isn't found in the dictionary. Try to parse the word as a number.
      char* endptr;
      int64_t num = strtoll(word, &endptr, 10);
      if (*endptr != '\0') {
        die("Unknown word: %s\n", word);
      }

      if (s->compiling) {
        // Store the entry for LITNUMBER following the number itself in memory.
        place_dict_word(s, "LITNUMBER");
        memcpy(&s->mem[s->here], &num, sizeof(int64_t));
        s->here += sizeof(int64_t);
      } else {
        push_data_stack(s, num);
      }
    }
  }
}