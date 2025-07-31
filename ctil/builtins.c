#include "builtins.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*builtin_func_t)(state_t*);

// TODO: reimplement _dot in Forth using lower primitives
void _dot(state_t* s) {
  assert(s->stacktop >= 0);
  fprintf(s->output, "%ld \n", s->stack[s->stacktop--]);
}

void key(state_t* s) {
  int c = fgetc(s->input);
  if (c == EOF) {
    exit(0);
  }
  s->stacktop++;
  s->stack[s->stacktop] = c;
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
  register_builtin(state, "DROP", 0, drop);
  register_builtin(state, "SWAP", 0, swap);
  register_builtin(state, "DUP", 0, dup);
}
