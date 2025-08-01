#include "builtins.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: skip compiling part for now. Try to get to executing simple
// interpreted programs and build tests based on that.

// TODO: reimplement _dot in Forth using lower primitives
void _dot(state_t* s) {
  assert(s->stacktop >= 0);
  fprintf(s->output, "%ld \n", s->stack[s->stacktop--]);
}

void emit(state_t* s) {
  assert(s->stacktop >= 0 && s->stack[s->stacktop] <= 255);
  int64_t c = s->stack[s->stacktop--];
  fputc((char)c, s->output);
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

// TODO: is word being used?
// TODO: rewrite this using get_word
// Read a word from the input stream into an internal buffer; push
// [addr, len] onto the stack.
// Note: the word is converted to uppercase while reading. If you need
// case-sensitive reading of words, don't use this builtin.
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
    buffer[writeptr++] = toupper(c);
    c = fgetc(s->input);
  }

  if (writeptr == 0) {
    exit(0);
  }
  s->stacktop++;
  s->stack[s->stacktop] = (int64_t)buffer;
  s->stacktop++;
  s->stack[s->stacktop] = writeptr;
}

// Expects [addr, len] of string on the stack. Finds a dictionary entry
// in 's' that matches the string and pushes its address onto the stack.
// If no match is found, pushes 0.
// void find(state_t* s) {
//   assert(s->stacktop >= 1);
//   int64_t slen = s->stack[s->stacktop--];
//   int64_t saddr = s->stack[s->stacktop--];

//   // Walking back the linked list of entries starting from latest.
//   int64_t entry_offset = s->latest;
//   while (entry_offset != -1) {
//     char* entry_name = &s->mem[entry_offset + 10];
//     if (strcmp(entry_name, (char*)saddr) == 0) {
//       // Found a match, push the address of the entry.
//       s->stacktop++;
//       s->stack[s->stacktop] = entry_offset;
//       return;
//     }
//     entry_offset = *(int64_t*)&s->mem[entry_offset];
//   }

//   // No match found, push 0.
//   s->stacktop++;
//   s->stack[s->stacktop] = 0;
// }

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
  register_builtin(state, "EMIT", 0, emit);
  register_builtin(state, "KEY", 0, key);
  register_builtin(state, "WORD", 0, word);
  //   register_builtin(state, "FIND", 0, find);
  register_builtin(state, "DROP", 0, drop);
  register_builtin(state, "SWAP", 0, swap);
  register_builtin(state, "DUP", 0, dup);
}
