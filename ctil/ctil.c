#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "die.h"
#include "state.h"

// A simple Forth interpreter, reading the Forth source code from stdin and
// printing the program's output to stdout.
int main(int argc, char* argv[]) {
  state_t* state = create_state();
  register_builtins(state);

  // Parse command-line arguments.
  // -i specifies an input file to read prior to interpreting stdin. This
  // is useful for loading the Forth "prelude" or other definitions.
  char* input_filename = NULL;
  for (int i = 1; i < argc - 1; ++i) {
    if (strcmp(argv[i], "-i") == 0) {
      input_filename = argv[i + 1];
      break;
    }
  }

  // If an input file was specified, open an interpret it before stdin.
  if (input_filename != NULL) {
    FILE* input_file = fopen(input_filename, "r");
    if (!input_file) {
      die("Could not open input file '%s'", input_filename);
    }
    state->input = input_file;
    interpret(state);
    fclose(input_file);
  }

  state->input = stdin;
  interpret(state);
  return 0;
}
