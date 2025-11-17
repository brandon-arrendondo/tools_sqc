/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: Preprocessor conditional spanning macro argument boundary
 */

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void func(int x, int y) {
#ifdef SPECIAL_MODE
    int result = MIN(x,  // Line 11 - VIOLATION
#endif
        y);
}

int main(void) {
    func(10, 20);
    return 0;
}
