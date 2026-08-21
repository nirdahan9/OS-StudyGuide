#include <stdio.h>
#include <stdlib.h>
#include "jump.h"

char test_stack[4096];

void dummy_entry(void) {
    printf("dummy_entry called\n");
}

int main() {
    printf("Before setup_jmpbuff\n");
    sigjmp_buf env;
    setup_jmpbuff(&env, test_stack, 4096, dummy_entry);
    printf("After setup_jmpbuff\n");
    return 0;
}
