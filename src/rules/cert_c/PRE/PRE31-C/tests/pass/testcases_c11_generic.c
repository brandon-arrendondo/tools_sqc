/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: C11 _Generic ensures argument evaluated once
 */

#include <math.h>

static inline int iabs(int v) {
    return v < 0 ? -v : v;
}

static inline long labs(long v) {
    return v < 0 ? -v : v;
}

// Safe: _Generic doesn't evaluate controlling expression
#define ABS(v) _Generic(v, int : iabs, \
                           long : labs, \
                           float : fabsf, \
                           double : fabs)(v)

void func(int n) {
    // Safe with side effects - COMPLIANT
    int m = ABS(++n);
}

int main(void) {
    func(5);
    return 0;
}
