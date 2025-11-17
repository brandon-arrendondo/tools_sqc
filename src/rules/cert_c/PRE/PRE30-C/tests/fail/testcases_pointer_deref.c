/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN for pointer dereference through concatenation
 */

#define DEREF(ptr1, ptr2) *(ptr1##ptr2)  // Line 7 - VIOLATION

void pointer_test(void) {
    int \u0450 = 50;  // Cyrillic small letter ie with grave
    int *p = &\u0450;

    // Creates \u0450 through concatenation
    int value = DEREF(\u04, 50);  // Line 14 - VIOLATION
}

int main(void) {
    pointer_test();
    return 0;
}
