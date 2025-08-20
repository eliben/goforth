#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdio.h>

#define MEM_SIZE (64 * 1024)
#define STACK_SIZE (64 * 1024)
#define MAX_NESTED_LOOPS 16
#define MAX_COMPILE_BACKPATCHES 16

// loop_compile_entry_t - a single entry in the loop compile stack.
typedef struct {
  // TODO terminology: use addr for absolute addresses in memory, and offset
  // for relative offsets.

  // Address of the loop body (the word following DO).
  int64_t start_offset;

  // A list of backpatch addresses, which are updated when we find the loop
  // end. The backpatches store the address of the word after the loop, used
  // to skip the loop body by words ?DO and LEAVE.
  int64_t backpatch_offsets[MAX_COMPILE_BACKPATCHES];
  int backpatch_count;
} loop_compile_entry_t;

typedef struct {
  // Forth memory where the program and the data are stored.
  char mem[MEM_SIZE];

  // Offset in mem to the latest defined word.
  int64_t latest;

  // Offset in mem where the next word can be defined.
  int64_t here;

  // Forth data stack, and a pointer to its top item.
  int64_t stack[STACK_SIZE];
  int64_t stacktop;

  // Program counter: the addr where the current word is being executed.
  int64_t pc;

  // Compiling mode: if non-zero, the interpreter is "compiling" (defining
  // a new word) rather than executing it directly.
  int compiling;

  // Forth return stack, and a pointer to its top item.
  int64_t retstack[STACK_SIZE];
  int64_t retstacktop;

  // Loop compile stack, used to compile counted loops (Do...LOOP and its
  // variants). The top of the stack points to the entry of the current loop
  // nesting while compiling.
  loop_compile_entry_t loop_compile_stack[MAX_NESTED_LOOPS];
  int loop_compile_stack_top;

  // Input and output streams.
  FILE* input;
  FILE* output;
} state_t;

// The function type of builtin functions implemented in C.
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

// Flags for dictionary entries.

// F_BUILTIN: if set, the entry is a builtin function implemented in C.
#define F_BUILTIN 0x80

// F_IMMEDIATE: if set, the entry is an immediate word that is executed
// immediately when encountered, rather than being compiled into the current
// definition.
#define F_IMMEDIATE 0x01

// Create a new interpreter state.
// Note: this doesn't set the input stream; this has to be set manually
// before 'interpret' is called.
state_t* create_state();

// Show the state and a memory dump.
void debug_dump_mem(state_t* s, uintptr_t start, uintptr_t len);

// Show the dictionary entries in the state.
void debug_dump_dict(state_t* s);

// Top-level Forth interpreter consuming the input stream.
void interpret(state_t* s);

// Place a dictionary word address at the current 'here' offset.
void place_dict_word(state_t* s, const char* word);

// Pushes value onto the data stack; asserts that the stack is not full.
void push_data_stack(state_t* s, int64_t value);

// Pops a value from the data stack; asserts that the stack is not empty.
int64_t pop_data_stack(state_t* s);

// Pushes a new loop entry item onto the stack, initialized with 0 backpatches.
// Returns its index.
size_t push_new_loop_entry(state_t* s, int64_t start_offset);

// Pops a value from the loop entry stack.
loop_compile_entry_t pop_loop_entry(state_t* s);

#endif // STATE_H