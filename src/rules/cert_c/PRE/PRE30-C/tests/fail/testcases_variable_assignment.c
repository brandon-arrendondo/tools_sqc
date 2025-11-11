/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN through concatenation for variable assignment
 */

#define SET_VAR(part1, part2, value) part1##part2 = value  // Line 7 - VIOLATION

void assign_variable(void) {
    int \u03B1;  // Greek small letter alpha

    // Forms \u03B1 via concatenation
    SET_VAR(\u03, B1, 15);  // Line 13 - VIOLATION
}

int main(void) {
    assign_variable();
    return 0;
}
