#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
  char mem[64 * 1024];

  // Offset in mem to the latest defined word.
  int64_t latest;

  // Offset in mem where the next word can be defined.
  int64_t here;

  // Forth data stack, and a pointer to its top item.
  int64_t stack[64 * 1024];
  int64_t stacktop;

  int64_t pc;

  // Compiling mode: if non-zero, the interpreter is "compiling" (defining
  // a new word) rather than executing it directly.
  int compiling;

  // Forth return stack, and a pointer to its top item.
  int64_t retstack[64 * 1024];
  int64_t retstacktop;

  // Input and output streams.
  FILE* input;
  FILE* output;
} state_t;

typedef void (*builtin_func_t)(state_t*);

// Dictionary entry:
// - Link offset (8 bytes)
// - Flags (1 byte)
// - Name length (1 byte)
// - Name
//   - The name is a null-terminated string, and space is allocated for it
//     until the next 8-byte boundary. This length is stored in the previous
//     byte.
// - Code (variable length)

#define F_BUILTIN 0x80
#define F_IMMEDIATE 0x40

// Create a new state.
state_t* create_state();

// Show the state and a memory dump.
void show_state(state_t* s, uintptr_t start, uintptr_t len);

// Top-level Forth interpreter consuming the input stream.
void interpret(state_t* s);

// Find a word in the dictionary by its name. Returns the offset of the
// dictionary entry in mem if found, or -1 if not found.
int64_t find_word_in_dict(state_t* s, const char* word);


#endif // STATE_H