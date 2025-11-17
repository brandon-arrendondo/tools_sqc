/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: rand() (side effect) in unsafe macro
 */

#include <stdlib.h>

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

void generate_random(void) {
    // rand() has side effect (changes state) - called multiple times
    int result = ABS(rand());  // Line 13 - VIOLATION
}

int main(void) {
    generate_random();
    return 0;
}
