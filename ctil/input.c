#include "input.h"

#include <assert.h>
#include <ctype.h>

size_t get_word(FILE* stream, char* outptr, size_t max_len) {
  assert(max_len > 0);
  assert(outptr != NULL);
  size_t len = 0;
  int c;

  // Skip leading whitespace.
  while ((c = fgetc(stream)) != EOF && isspace(c)) {
  }

  // Read the word until whitespace or EOF.
  while (c != EOF && !isspace(c) && len < max_len - 1) {
    outptr[len++] = (char)toupper(c);
    c = fgetc(stream);
  }

  outptr[len] = '\0'; // Null-terminate the string.
  return len;
}
