#ifndef BUILTINS_H
#define BUILTINS_H

#include "state.h"

// Register all builtin functions with the state.
void register_builtins(state_t* state);


typedef void (*builtin_func_t)(state_t*);

#endif // BUILTINS_H
