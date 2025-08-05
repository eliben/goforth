#include "builtins.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "die.h"
#include "input.h"

// Align a name length to 8 bytes, including the null terminator.
static uint8_t align_name_len(uint8_t len) {
  len++;
  if (len % 8 != 0) {
    len += 8 - (len % 8);
  }
  return len;
}

// TODO: once more machinery is in place, consider rewriting some of these
// in Forth directly.

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
  assert(s->stacktop >= 0);
  fprintf(s->output, "%ld ", s->stack[s->stacktop--]);
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

  s->stacktop++;
  s->stack[s->stacktop] = v2;
  s->stacktop++;
  s->stack[s->stacktop] = v1;
}

void drop2(state_t* s) {
  assert(s->stacktop >= 1);
  s->stacktop -= 2;
}

void key(state_t* s) {
  int c = fgetc(s->input);
  if (c == EOF) {
    exit(0);
  }
  s->stacktop++;
  s->stack[s->stacktop] = c;
}

void plus(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  s->stacktop++;
  s->stack[s->stacktop] = b + a;
}

void minus(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  s->stacktop++;
  s->stack[s->stacktop] = b - a;
}

void mod(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  assert(b != 0);
  s->stacktop++;
  s->stack[s->stacktop] = b % a;
}

void mul(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  s->stacktop++;
  s->stack[s->stacktop] = b * a;
}

void _div(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  assert(b != 0);
  s->stacktop++;
  s->stack[s->stacktop] = b / a;
}

void _equals(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  s->stacktop++;
  s->stack[s->stacktop] = (b == a) ? -1 : 0;
}

void _notequals(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  s->stacktop++;
  s->stack[s->stacktop] = (b != a) ? -1 : 0;
}

void _lt(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  s->stacktop++;
  s->stack[s->stacktop] = (b < a) ? -1 : 0;
}

void _gt(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t a = s->stack[s->stacktop--];
  int64_t b = s->stack[s->stacktop--];
  s->stacktop++;
  s->stack[s->stacktop] = (b > a) ? -1 : 0;
}

// litnumber is a special builtin emitted in compile mode; it pushes the number
// stored at the next pc onto the stack and advances the pc.
void litnumber(state_t* s) {
  s->stacktop++;
  s->pc += sizeof(int64_t);
  s->stack[s->stacktop] = *(int64_t*)&s->mem[s->pc];
}

// void litstring(state_t* s) {

//   // Push the string onto the stack.
//   s->stacktop++;
//   s->stack[s->stacktop] = (int64_t)strndup((char*)addr, len);
// }

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

// In JonesForth, this is called CREATE, but it's not a standard Forthe CREATE,
// so we give it a special name.
void createdef(state_t* s) {
  assert(s->stacktop >= 1);
  int64_t len = s->stack[s->stacktop--];
  int64_t addr = s->stack[s->stacktop--];

  // Build a new dictionary entry.
  memcpy(&s->mem[s->here], &s->latest, sizeof(int64_t));
  s->latest = s->here;
  s->here += sizeof(int64_t);
  s->mem[s->here++] = 0;

  uint8_t name_len = align_name_len((uint8_t)len);
  s->mem[s->here++] = name_len;
  strncpy(&s->mem[s->here], (char*)addr, len);
  s->mem[s->here + len] = '\0';
  s->here += name_len;
}

void comma(state_t* s) {
  assert(s->stacktop >= 0);
  int64_t value = s->stack[s->stacktop--];

  // Store the value in the memory at the current position.
  memcpy(&s->mem[s->here], &value, sizeof(int64_t));
  s->here += sizeof(int64_t);
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
  register_builtin(state, ".\"", 0, dotQuote);
  register_builtin(state, ".S", 0, dotS);
  register_builtin(state, "EMIT", 0, emit);
  register_builtin(state, "KEY", 0, key);
  //   register_builtin(state, "FIND", 0, find);
  register_builtin(state, "DROP", 0, drop);
  register_builtin(state, "SWAP", 0, swap);
  register_builtin(state, "DUP", 0, dup);
  register_builtin(state, "OVER", 0, over);
  register_builtin(state, "2DUP", 0, dup2);
  register_builtin(state, "2DROP", 0, drop2);
  register_builtin(state, "+", 0, plus);
  register_builtin(state, "-", 0, minus);
  register_builtin(state, "*", 0, mul);
  register_builtin(state, "/", 0, _div);
  register_builtin(state, "MOD", 0, mod);
  register_builtin(state, "=", 0, _equals);
  register_builtin(state, "<>", 0, _notequals);
  register_builtin(state, "<", 0, _lt);
  register_builtin(state, ">", 0, _gt);

  register_builtin(state, "LITNUMBER", 0, litnumber);
  //   register_builtin(state, "LITSTRING", 0, litstring);
  register_builtin(state, "WORD", 0, word);
  register_builtin(state, "CREATEDEF", 0, createdef);
  register_builtin(state, ",", 0, comma);
  register_builtin(state, ":", 0, colon);
  register_builtin(state, ";", F_IMMEDIATE, semicolon);

  // TODO: remove this when almost done
  // Add non-builtin word
  //    : double dup + ;
  memcpy(&state->mem[state->here], &state->latest, sizeof(int64_t));
  state->latest = state->here;
  state->here += sizeof(int64_t);
  state->mem[state->here++] = 0;
  uint8_t name_len = 8;
  state->mem[state->here++] = name_len;
  strcpy(&state->mem[state->here], "DOUBLE");
  state->here += name_len;

  int64_t dup_offset = find_word_in_dict(state, "DUP");
  assert(dup_offset != -1);
  memcpy(&state->mem[state->here], &dup_offset, sizeof(int64_t));
  state->here += sizeof(int64_t);

  int64_t plus_offset = find_word_in_dict(state, "+");
  assert(plus_offset != -1);
  memcpy(&state->mem[state->here], &plus_offset, sizeof(int64_t));
  state->here += sizeof(int64_t);

  int64_t end_marker = -1;
  memcpy(&state->mem[state->here], &end_marker, sizeof(int64_t));
  state->here += sizeof(int64_t);
}
