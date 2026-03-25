/*
 * Rule: DCL02-C
 * Source: testcases
 * Status: PASS - Known limitation: function-scope declarations not checked
 * TODO: Move to fail/ when function-scope similarity checking is implemented (see PLAN.md)
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
