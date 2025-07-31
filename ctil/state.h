#ifndef STATE_H
#define STATE_H

#include <stdint.h>

typedef struct {
  char mem[64 * 1024];

  // Offset in mem to the latest defined word.
  int64_t latest;

  // Offset in mem where the next word can be defined.
  int64_t here;

  // Current program counter (offset in mem).
  int64_t pc;

  // Forth data stack.
  int64_t stack[64 * 1024];

  // Pointer to the top item on the data stack.
  int64_t stacktop;

} state_t;

// Dictionary entry:
// - Link offset (8 bytes)
// - Flags (1 byte)
// - Name length (1 byte)
// - Name
//   - The name is a null-terminated string, and space is allocated for it
//     until the next 8-byte boundary. This length is stored in the previous
//     byte.
// - Code (variable length)

#define F_BUILTIN       0x80
#define F_IMMEDIATE     0x40

// Create a new state.
state_t* create_state();

// Show the state and a memory dump.
void show_state(state_t* s, uintptr_t start, uintptr_t len);

#endif // STATE_H