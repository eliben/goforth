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

  // Current program counter (offset in mem).
  int64_t pc;

  // Forth data stack, and a pointer to its top item.
  int64_t stack[64 * 1024];
  int64_t stacktop;

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

// TODO instead of F_BUILTIN, we should probably have an "interpreter" for
// forth words like DOCOL, it should use pc to find which word to execute.
// The whole thing should be an interpreter, instead of relying on the C
// call stack. Calls into C should only happen for built-ins.
#define F_BUILTIN 0x80
#define F_IMMEDIATE 0x40

// Create a new state.
state_t* create_state();

// Show the state and a memory dump.
void show_state(state_t* s, uintptr_t start, uintptr_t len);

// Find a word in the dictionary by its name. Returns the offset of the
// dictionary entry in mem if found, or -1 if not found.
int64_t find_word_in_dict(state_t* s, const char* word);

// Execute the word found at the given memory offset.
void execute_word(state_t* s, int64_t entry_offset);

#endif // STATE_H