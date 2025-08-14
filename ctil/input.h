#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

// Get the next whitespace-separated word from the input stream, without
// converting it to uppercase. Writes the word into outptr (up to max_len
// characters) and returns its length. Returns 0 if EOF is reached before any
size_t get_word_case_sensitive(FILE* stream, char* outptr, size_t max_len);

// Like get_word_case_sensitive, but converts the word to uppercase
// while reading.
size_t get_word(FILE* stream, char* outptr, size_t max_len);

#endif // INPUT_H