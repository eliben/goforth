#include "state.h"

#include "zmalloc.h"

state_t* create_state() {
  state_t* s = (state_t*)zmalloc(sizeof(state_t));
  s->pc = 0;
  s->latest = 0;
  s->here = 0;
  s->stacktop = -1;
  return s;
}
