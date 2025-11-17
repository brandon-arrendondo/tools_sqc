/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in switch statement through concatenation
 */

#define SWITCH_VAR(v1, v2) switch(v1##v2)  // Line 7 - VIOLATION

void switch_test(void) {
    int \u04F0 = 2;  // Cyrillic capital letter u with diaeresis

    // Creates \u04F0 through concatenation
    SWITCH_VAR(\u04, F0) {  // Line 13 - VIOLATION
        case 2:
            break;
    }
}

int main(void) {
    switch_test();
    return 0;
}
