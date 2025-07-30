#include "builtins.h"

#include <assert.h>
#include <string.h>


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
    // Manual definition of DROP, just for testing.
    memcpy(&state->mem[state->here], &state->latest, sizeof(int64_t));
    state->latest = (int64_t)&state->mem[state->here];
    state->here += sizeof(int64_t);
    state->mem[state->here++] = (char)0x80; // top bit set for builtin

    uint8_t namelen = 4 + 1;
    // align namelen to 8
    if (namelen % 8 != 0) {
        namelen += 8 - (namelen % 8);
    }

    state->mem[state->here++] = namelen;
    strcpy(&state->mem[state->here], "DROP");
    state->here += namelen;

    // Store pointer to the function.
    builtin_func_t func = drop;
    memcpy(&state->mem[state->here], &func, sizeof(builtin_func_t));
    state->here += sizeof(builtin_func_t);
}
