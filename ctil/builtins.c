#include "builtins.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "die.h"
#include "input.h"

// TODO: once more machinery is in place, try moving as much as possible into
// the prelude.

// Align a name length to 8 bytes, including the null terminator.
static uint8_t align_name_len(uint8_t len) {
  len++;
  if (len % 8 != 0) {
    len += 8 - (len % 8);
  }
  return len;
}

void backslash(state_t* s) {
  int c;
  while ((c = fgetc(s->input)) != EOF && c != '\n') {
  }
}

void paren(state_t* s) {
  int c;
  while ((c = fgetc(s->input)) != EOF && c != ')') {
  }
  if (c == EOF) {
    die("Unmatched parenthesis in input");
  }
}

void dot(state_t* s) {
  fprintf(s->output, "%ld ", pop_data_stack(s));
}

void dotQuote(state_t* s) {
  char buf[1024];
  size_t len = 0;
  int c;
  while ((c = fgetc(s->input)) != EOF && c != '"') {
    if (len < sizeof(buf) - 1) {
      buf[len++] = c;
    }
  }
  if (c == EOF) {
    die("Unmatched quote in input");
  }
  buf[len] = '\0';

  if (s->compiling) {
    // Store the following in memory:
    //
    // <addr of LITSTRING> <str length> <string> <addr of TYPE>
    //
    // The string is 0-terminated, but <str length> is aligned to 8 bytes
    // to it can be efficiently skipped.
    int64_t litstring_offset = find_word_in_dict(s, "LITSTRING");
    assert(litstring_offset != -1);
    int64_t type_offset = find_word_in_dict(s, "TYPE");
    assert(type_offset != -1);

    memcpy(&s->mem[s->here], &litstring_offset, sizeof(int64_t));
    s->here += sizeof(int64_t);
    int64_t str_len = align_name_len((uint8_t)len);
    memcpy(&s->mem[s->here], &str_len, sizeof(int64_t));
    s->here += sizeof(int64_t);
    memcpy(&s->mem[s->here], buf, len);
    s->here += str_len;
    memcpy(&s->mem[s->here], &type_offset, sizeof(int64_t));
    s->here += sizeof(int64_t);
  } else {
    fprintf(s->output, "%s", buf);
  }
}

void dotS(state_t* s) {
  int64_t len = s->stacktop + 1;
  fprintf(s->output, "<%ld> ", len);
  for (int64_t i = 0; i <= s->stacktop; i++) {
    fprintf(s->output, "%ld ", s->stack[i]);
  }
}

// .D dumps the interpreter state to stdout, for debugging.
void dotD(state_t* s) {
    debug_dump_mem(s, 0, s->here);
    debug_dump_dict(s);
}

void clearstack(state_t* s) {
  s->stacktop = -1;
}

void emit(state_t* s) {
  fputc((char)pop_data_stack(s), s->output);
}

// type prints a 0-terminated string from memory.
void type(state_t* s) {
  pop_data_stack(s); // Pop the length, which we don't use.
  int64_t addr = pop_data_stack(s);
  fprintf(s->output, "%s", &s->mem[addr]);
}

void drop(state_t* s) {
  pop_data_stack(s);
}

void swap(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t temp = s->stack[s->stacktop];
  s->stack[s->stacktop] = s->stack[s->stacktop - 1];
  s->stack[s->stacktop - 1] = temp;
}

void over(state_t* s) {
  assert(s->stacktop >= 1);
  s->stacktop++;
  s->stack[s->stacktop] = s->stack[s->stacktop - 2];
}

void dup(state_t* s) {
  assert(s->stacktop >= 0);
  s->stacktop++;
  s->stack[s->stacktop] = s->stack[s->stacktop - 1];
}

void dup2(state_t* s) {
  assert(s->stacktop >= 1);
  // [v2 v1 -- v2 v1 v2 v1]
  int64_t v1 = s->stack[s->stacktop];
  int64_t v2 = s->stack[s->stacktop - 1];

  push_data_stack(s, v2);
  push_data_stack(s, v1);
}

void drop2(state_t* s) {
  pop_data_stack(s);
  pop_data_stack(s);
}

// Pop a value from the return stack and push it onto the data stack.
void fromR(state_t* s) {
  assert(s->retstacktop >= 0);
  push_data_stack(s, s->retstack[s->retstacktop--]);
}

// Pop a value from the data stack and push it onto the return stack.
void toR(state_t* s) {
  s->retstacktop++;
  s->retstack[s->retstacktop] = pop_data_stack(s);
}

// Copy the top value from the return stack to the data stack without removing
// it from the return stack.
void copyFromR(state_t* s) {
  assert(s->retstacktop >= 0);
  push_data_stack(s, s->retstack[s->retstacktop]);
}

// Drop the top value from the return stack.
void dropR(state_t* s) {
  assert(s->retstacktop >= 0);
  s->retstacktop--;
}

void key(state_t* s) {
  int c = fgetc(s->input);
  if (c == EOF) {
    exit(0);
  }
  push_data_stack(s, c);
}

void plus(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  push_data_stack(s, b + a);
}

void minus(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  push_data_stack(s, b - a);
}

void divmod(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  assert(a != 0);
  push_data_stack(s, b % a);
  push_data_stack(s, b / a);
}

void mul(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  push_data_stack(s, b * a);
}

void _equals(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  push_data_stack(s, (b == a) ? -1 : 0);
}

void _notequals(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  push_data_stack(s, (b != a) ? -1 : 0);
}

void _lt(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  push_data_stack(s, (b < a) ? -1 : 0);
}

void _gt(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = pop_data_stack(s);
  int64_t b = pop_data_stack(s);
  push_data_stack(s, (b > a) ? -1 : 0);
}

// litnumber is a special builtin emitted in compile mode; it pushes the number
// stored at the next pc onto the stack and advances the pc.
void litnumber(state_t* s) {
  s->pc += sizeof(int64_t);
  push_data_stack(s, *(int64_t*)&s->mem[s->pc]);
}

// litstring is a special builtin emitted in compile mode. It's followed by
// the length of a string and the string itself in memory. It pushes the
// address and length onto the stack and skips the string.
void litstring(state_t* s) {
  s->pc += sizeof(int64_t);
  int64_t len = *(int64_t*)&s->mem[s->pc];
  s->pc += sizeof(int64_t);
  int64_t addr = s->pc;

  push_data_stack(s, addr);
  push_data_stack(s, len);

  // Skip the string, moving to its last word. After builtins are executed,
  // s->pc is always bumped by one word.
  s->pc += len - sizeof(int64_t);
}

// Unconditional branch to pc+offset, where offset is the next word in memory.
void branch(state_t* s) {
  s->pc += sizeof(int64_t);
  int64_t offset = *(int64_t*)&s->mem[s->pc];
  s->pc += offset;
}

// Conditional branch to pc+offset, where offset is the next word in memory
// and a flag is on TOS. The branch happens if the flag is zero.
void branch0(state_t* s) {
  s->pc += sizeof(int64_t);
  int64_t offset = *(int64_t*)&s->mem[s->pc];
  s->pc += sizeof(int64_t);

  int64_t flag = pop_data_stack(s);
  if (flag == 0) {
    s->pc += offset;
  }
}

// Create reads the next word from the input stream and creates a new
// dictionary entry for it. The word contains a LITNUMBER <addr> in it,
// where <addr> is the address of the memory location HERE points to just
// after the dictionary definition. This is usually used in combination
// with COMMA or ALLOT to allocate space for the word's data.
//
// When CREATE is compiled into another word's definition, when it's executed
// it reads the next word from the input stream at the point of execution.
// Therefore it's _not_ IMMEDIATE.
// See the definition of VARIABLE for an example.
void create(state_t* s) {
  // Create a new dictionary entry for a word.
  char buf[256];
  size_t len = get_word(s->input, buf, sizeof(buf));
  if (len == 0) {
    die("Error: expected a word name after 'CREATE'");
  }

  // Build a new dictionary entry.
  memcpy(&s->mem[s->here], &s->latest, sizeof(int64_t));
  s->latest = s->here;
  s->here += sizeof(int64_t);
  s->mem[s->here++] = 0;

  uint8_t name_len = align_name_len((uint8_t)len);
  s->mem[s->here++] = name_len;
  strncpy(&s->mem[s->here], buf, len);
  s->mem[s->here + len] = '\0';
  s->here += name_len;

  // We emit a word that has LITNUMBER <addr> in it; the <addr> points to
  // the here following this definition in memory.
  int64_t litnumber_offset = find_word_in_dict(s, "LITNUMBER");
  assert(litnumber_offset != -1);
  memcpy(&s->mem[s->here], &litnumber_offset, sizeof(int64_t));
  s->here += sizeof(int64_t);

  int64_t store_offset = s->here + 2 * sizeof(int64_t);
  memcpy(&s->mem[s->here], &store_offset, sizeof(int64_t));
  s->here += sizeof(int64_t);

  int64_t end_marker = -1;
  memcpy(&s->mem[s->here], &end_marker, sizeof(int64_t));
  s->here += sizeof(int64_t);
}

// Read a word from the input stream and push the value of its first character
// onto the stack.
void _char(state_t* s) {
  char buf[256];
  size_t len = get_word_case_sensitive(s->input, buf, sizeof(buf));
  if (len == 0) {
    die("Error: expected a word name after 'CHAR'");
  }
  push_data_stack(s, (int64_t)buf[0]);
}

void allot(state_t* s) {
  // Allocate space in memory for a new word.
  int64_t size = pop_data_stack(s);
  if (s->here + size > sizeof(s->mem)) {
    die("Memory allocation out of bounds: %ld requested, %ld available", size,
        sizeof(s->mem) - s->here);
  }
  s->here += size;
}

void at(state_t* s) {
  int64_t addr = pop_data_stack(s);

  // Read the value from memory at the given address.
  if (addr < 0 || addr >= sizeof(s->mem) - sizeof(int64_t)) {
    die("Memory access out of bounds: %ld", addr);
  }
  int64_t value;
  memcpy(&value, &s->mem[addr], sizeof(int64_t));
  push_data_stack(s, value);
}

void exclamation(state_t* s) {
  int64_t addr = pop_data_stack(s);
  int64_t value = pop_data_stack(s);

  // Write the value to memory at the given address.
  if (addr < 0 || addr >= sizeof(s->mem) - sizeof(int64_t)) {
    die("Memory access out of bounds: %ld", addr);
  }
  memcpy(&s->mem[addr], &value, sizeof(int64_t));
}

void cAt(state_t* s) {
  int64_t addr = pop_data_stack(s);

  // Read a byte from memory at the given address.
  if (addr < 0 || addr >= sizeof(s->mem)) {
    die("Memory access out of bounds: %ld", addr);
  }
  char value = s->mem[addr];
  push_data_stack(s, (int64_t)value);
}

void cExclamation(state_t* s) {
  int64_t addr = pop_data_stack(s);
  int64_t value = pop_data_stack(s);

  // Write a byte to memory at the given address.
  if (addr < 0 || addr >= sizeof(s->mem)) {
    die("Memory access out of bounds: %ld", addr);
  }
  s->mem[addr] = (char)value;
}

void question(state_t* s) {
  int64_t addr = pop_data_stack(s);

  if (addr < 0 || addr >= sizeof(s->mem) - sizeof(int64_t)) {
    die("Memory access out of bounds: %ld", addr);
  }

  fprintf(s->output, "%ld ", *(int64_t*)&s->mem[addr]);
}

void colon(state_t* s) {
  char buf[256];
  size_t len = get_word(s->input, buf, sizeof(buf));
  if (len == 0) {
    die("Error: expected a word name after ':'");
  }
  if (strcmp(buf, ";") == 0) {
    // Empty definition: no op.
    return;
  }

  // Build a new dictionary entry.
  memcpy(&s->mem[s->here], &s->latest, sizeof(int64_t));
  s->latest = s->here;
  s->here += sizeof(int64_t);
  s->mem[s->here++] = 0;

  uint8_t name_len = align_name_len((uint8_t)len);
  s->mem[s->here++] = name_len;
  strcpy(&s->mem[s->here], buf);
  s->here += name_len;
  s->compiling = 1;
}

void semicolon(state_t* s) {
  // Finish the current definition and switch back to execution mode.
  int64_t end_marker = -1;
  memcpy(&s->mem[s->here], &end_marker, sizeof(int64_t));
  s->here += sizeof(int64_t);
  s->compiling = 0;
}

void tick(state_t* s) {
  s->pc += sizeof(int64_t);
  push_data_stack(s, *(int64_t*)&s->mem[s->pc]);
}

void comma(state_t* s) {
  int64_t value = pop_data_stack(s);
  memcpy(&s->mem[s->here], &value, sizeof(int64_t));
  s->here += sizeof(int64_t);
}

void here(state_t* s) {
  push_data_stack(s, s->here);
}

void immediate(state_t* s) {
  // latest points at the start of the word we're defining. Use it to find
  // the flag field and set the F_IMMEDIATE flag.
  size_t flag_offset = s->latest + sizeof(int64_t);
  s->mem[flag_offset] |= F_IMMEDIATE;
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

  uint8_t namelen = align_name_len((uint8_t)strlen(name));
  state->mem[state->here++] = namelen;
  strcpy(&state->mem[state->here], name);
  state->here += namelen;

  // Store pointer to the function.
  memcpy(&state->mem[state->here], &func, sizeof(builtin_func_t));
  state->here += sizeof(builtin_func_t);
}

void register_builtins(state_t* state) {
  register_builtin(state, "\\", F_IMMEDIATE, backslash);
  register_builtin(state, "(", F_IMMEDIATE, paren);
  register_builtin(state, ".", 0, dot);
  register_builtin(state, ".\"", F_IMMEDIATE, dotQuote);
  register_builtin(state, ".S", 0, dotS);
  register_builtin(state, ".D", 0, dotD);
  register_builtin(state, "CLEARSTACK", 0, clearstack);
  register_builtin(state, "EMIT", 0, emit);
  register_builtin(state, "TYPE", 0, type);
  register_builtin(state, "KEY", 0, key);
  register_builtin(state, "DROP", 0, drop);
  register_builtin(state, "SWAP", 0, swap);
  register_builtin(state, "DUP", 0, dup);
  register_builtin(state, "OVER", 0, over);
  register_builtin(state, "2DUP", 0, dup2);
  register_builtin(state, "2DROP", 0, drop2);
  register_builtin(state, "R>", 0, fromR);
  register_builtin(state, ">R", 0, toR);
  register_builtin(state, "R@", 0, copyFromR);
  register_builtin(state, "RDROP", 0, dropR);
  register_builtin(state, "+", 0, plus);
  register_builtin(state, "-", 0, minus);
  register_builtin(state, "*", 0, mul);

  register_builtin(state, "/MOD", 0, divmod);
  register_builtin(state, "=", 0, _equals);
  register_builtin(state, "<>", 0, _notequals);
  register_builtin(state, "<", 0, _lt);
  register_builtin(state, ">", 0, _gt);

  register_builtin(state, "IMMEDIATE", F_IMMEDIATE, immediate);
  register_builtin(state, "'", 0, tick);
  register_builtin(state, ",", 0, comma);
  register_builtin(state, "HERE", 0, here);
  register_builtin(state, "LITNUMBER", 0, litnumber);
  register_builtin(state, "LITSTRING", 0, litstring);
  register_builtin(state, "BRANCH", 0, branch);
  register_builtin(state, "0BRANCH", 0, branch0);

  register_builtin(state, "CHAR", 0, _char);
  register_builtin(state, "CREATE", 0, create);

  register_builtin(state, ",", 0, comma);
  register_builtin(state, "@", 0, at);
  register_builtin(state, "!", 0, exclamation);
  register_builtin(state, "C@", 0, cAt);
  register_builtin(state, "C!", 0, cExclamation);
  register_builtin(state, "?", 0, question);
  register_builtin(state, "ALLOT", 0, allot);

  register_builtin(state, ":", 0, colon);
  register_builtin(state, ";", F_IMMEDIATE, semicolon);
}
