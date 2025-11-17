/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using char pointer with offsetof for byte arithmetic
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

    // Cast to unsigned char* for byte arithmetic - COMPLIANT
    unsigned char *ptr = (unsigned char *)malloc(sizeof(struct big));

    if (ptr != NULL) {
        memset(ptr + skip, 0, sizeof(struct big) - skip);
        free(ptr);
    }
}

int main(void) {
    func();
    return 0;
}
