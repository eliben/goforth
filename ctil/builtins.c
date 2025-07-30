#include "builtins.h"

#include <assert.h>


typedef void (*builtin_func_t)(state_t*);

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

void dup(state_t* s) {
    assert(s->stacktop >= 0);
    s->stacktop++;
    s->stack[s->stacktop] = s->stack[s->stacktop - 1];
}

void register_builtins(state_t* state) {
}
