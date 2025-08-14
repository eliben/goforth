#include "input.h"

#include <assert.h>
#include <ctype.h>

size_t get_word_case_sensitive(FILE* stream, char* outptr, size_t max_len) {
  assert(max_len > 0);
  assert(outptr != NULL);
  size_t len = 0;
  int c;

  // Skip leading whitespace.
  while ((c = fgetc(stream)) != EOF && isspace(c)) {
  }

  // Read the word until whitespace or EOF.
  while (c != EOF && !isspace(c) && len < max_len - 1) {
    outptr[len++] = (char)c;
    c = fgetc(stream);
  }

  outptr[len] = '\0'; // Null-terminate the string.
  return len;
}

size_t get_word(FILE* stream, char* outptr, size_t max_len) {
  size_t len = get_word_case_sensitive(stream, outptr, max_len);
  // Convert the word to uppercase.
  for (size_t i = 0; i < len; ++i) {
    outptr[i] = toupper(outptr[i]);
  }
  return len;
}