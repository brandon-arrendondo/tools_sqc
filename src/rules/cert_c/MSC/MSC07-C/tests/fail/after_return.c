// Test: code after unconditional return is unreachable
#include <stdio.h>

int foo(int x) {
    return x + 1;
    printf("unreachable\n");  // MSC07-C violation
}
