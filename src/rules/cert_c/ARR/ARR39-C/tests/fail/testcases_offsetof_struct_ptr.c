/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using offsetof() with typed pointer causes double-scaling
 */

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

struct big {
    unsigned long long ull_a;
    unsigned long long ull_b;
    unsigned long long ull_c;
    int si_d;
    int si_e;
};

void func(void) {
    size_t skip = offsetof(struct big, ull_b);
    struct big *s = (struct big *)malloc(sizeof(struct big));

    if (s != NULL) {
        // offsetof returns bytes, gets scaled again as struct big*
        memset(s + skip, 0, sizeof(struct big) - skip);  // Line 25 - VIOLATION

        free(s);
    }
}

int main(void) {
    func();
    return 0;
}
