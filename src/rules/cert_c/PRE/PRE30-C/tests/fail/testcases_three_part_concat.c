/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Concatenating three parts to form UCN
 */

#define JOIN3(a, b, c) a##b##c  // Line 7 - VIOLATION

void multi_part(void) {
    int \u0410;  // Cyrillic capital letter A

    // Creates \u0410 through three-way concatenation
    JOIN3(\u, 04, 10) = 20;  // Line 13 - VIOLATION
}

int main(void) {
    multi_part();
    return 0;
}
