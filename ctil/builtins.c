#include "builtins.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*builtin_func_t)(state_t*);

// TODO: reimplement _dot in Forth using lower primitives
void _dot(state_t* s) {
  assert(s->stacktop >= 0);
  fprintf(s->output, "%ld \n", s->stack[s->stacktop--]);
}

void drop(state_t* s) {
  assert(s->stacktop >= 0);
  s->stacktop--;
}

void swap(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t temp = s->stack[s->stacktop];
  s->stack[s->stacktop] = s->stack[s->stacktop - 1];
  s->stack[s->stacktop - 1] = temp;
}

void dup(state_t* s) {
  assert(s->stacktop >= 0);
  s->stacktop++;
  s->stack[s->stacktop] = s->stack[s->stacktop - 1];
}

void key(state_t* s) {
  int c = fgetc(s->input);
  if (c == EOF) {
    exit(0);
  }
  s->stacktop++;
  s->stack[s->stacktop] = c;
}

// Read a word from the input stream into an internal buffer; push
// [addr, len] onto the stack.
void word(state_t* s) {
  static char buffer[256];

  // Find beginning of the next word, skipping whitespace.
  char c;
  while ((c = fgetc(s->input)) != EOF) {
    if (isspace(c)) {
      continue;
    }
  }

  int writeptr = 0;
  // Read the word until whitespace or EOF.
  while (c != EOF && !isspace(c)) {
    assert(writeptr < sizeof(buffer) - 1);
    buffer[writeptr++] = c;
    c = fgetc(s->input);
  }

  if (writeptr == 0) {
    exit(0);
  }
  s->stack[s->stacktop++] = (int64_t)buffer;
  s->stack[s->stacktop++] = writeptr;
}

// Create a new dictionary entry for a built-in function. The F_BUILTIN flag
// is automatically set for all built-ins; the flags parameter can be used
// to specify additional flags.
static void register_builtin(state_t* state, const char* name, char flags,
                             builtin_func_t func) {
  memcpy(&state->mem[state->here], &state->latest, sizeof(int64_t));
  state->latest = state->here;
  state->here += sizeof(int64_t);
  state->mem[state->here++] = (char)(F_BUILTIN | flags);

  // Store the length of the name with the 0 terminator, aligned to 8 bytes.
  uint8_t namelen = (uint8_t)strlen(name) + 1;
  if (namelen % 8 != 0) {
    namelen += 8 - (namelen % 8);
  }
  state->mem[state->here++] = namelen;
  strcpy(&state->mem[state->here], name);
  state->here += namelen;

  // Store pointer to the function.
  memcpy(&state->mem[state->here], &func, sizeof(builtin_func_t));
  state->here += sizeof(builtin_func_t);
}

void register_builtins(state_t* state) {
  register_builtin(state, ".", 0, _dot);
  register_builtin(state, "KEY", 0, key);
  register_builtin(state, "WORD", 0, word);
  register_builtin(state, "DROP", 0, drop);
  register_builtin(state, "SWAP", 0, swap);
  register_builtin(state, "DUP", 0, dup);
}
