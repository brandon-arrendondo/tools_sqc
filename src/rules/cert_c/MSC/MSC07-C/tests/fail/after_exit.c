// Test: code after exit() is unreachable
#include <stdlib.h>
#include <stdio.h>

void fatal_error(void) {
    exit(1);
    printf("unreachable\n");  // MSC07-C violation
}
