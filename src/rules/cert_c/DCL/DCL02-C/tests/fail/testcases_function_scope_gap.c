/*
 * Rule: DCL02-C
 * Source: testcases
 * Status: FAIL - function-scope visually similar identifiers
 *
 * These identifiers differ only by 1/I (visually similar) but the rule currently
 * only checks file-scope declarations, not function-local ones.
 */

void test_function_scope(void) {
    int var1 = 0;
    int varI = 1;
    (void)var1;
    (void)varI;
}
