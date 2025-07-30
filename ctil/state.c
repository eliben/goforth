#include "state.h"

#include <stdio.h>
#include "zmalloc.h"

state_t* create_state() {
  state_t* s = (state_t*)zmalloc(sizeof(state_t));
  s->pc = 0;
  s->latest = 0;
  s->here = 0;
  s->stacktop = -1;
  return s;
}


void show_state(state_t* s, uintptr_t start, uintptr_t len) {
    // Print latest, here etc.
    printf("State:\n");
    printf("  latest: 0x%lx\n", (uint64_t)s->latest);
    printf("  here: 0x%lx\n", (uint64_t)s->here);

    // Print the memory region from start to start + len.
    printf("Memory dump from 0x%lx to 0x%lx:\n", start, start + len);
    for (uintptr_t i = start; i < start + len; i++) {
        if (i % 8 == 0) {
            if (i != start) {
                printf("\n");
            }
            printf("%08lx: ", i);
        }
        printf("%02x ", (unsigned char)s->mem[i]);
    }
    printf("\n");
}