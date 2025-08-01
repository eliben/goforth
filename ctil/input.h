#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

// Get the next whitespace-separated word from the input stream, converting it
// to uppercase in the process. Writes the word into outptr (up to max_len
// characters) and returns its length. Returns 0 if EOF is reached before any
// characters are read.
size_t get_word(FILE* stream, char* outptr, size_t max_len);

#endif // INPUT_H