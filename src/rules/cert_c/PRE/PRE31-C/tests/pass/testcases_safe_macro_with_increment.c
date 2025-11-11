/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: Safe macro (evaluates argument once) can handle side effects
 */

// Safe macro - evaluates argument exactly once
#define SAFE_ABS(x) ({ __typeof__(x) _tmp = (x); _tmp < 0 ? -_tmp : _tmp; })

void func(int n) {
    // Safe macro evaluates once - COMPLIANT
    int m = SAFE_ABS(++n);
}

int main(void) {
    func(5);
    return 0;
}
