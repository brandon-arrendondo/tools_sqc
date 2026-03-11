/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 *
 * Tests: for-loop initializer always executes before condition/body,
 * so variables assigned in the init clause are initialized within
 * the loop, even when the for-loop is inside a conditional.
 */

void process(int x);

/* For-init inside if-without-else: i is initialized by for-init */
void test_for_inside_if(int condition) {
    int i;
    if (condition) {
        for (i = 0; i < 10; i++) {
            process(i);
        }
    }
}

/* For-init with comma expression */
void test_for_comma_init(int n) {
    int i, sum;
    for (i = 0, sum = 0; i < n; i++) {
        sum += i;
        process(sum);
    }
}

/* Inline for declaration inside conditional */
void test_for_inline_decl_in_if(int condition, int n) {
    if (condition) {
        for (int j = 0; j < n; j++) {
            process(j);
        }
    }
}

/* Variable assigned in parent scope, read in nested scope */
void test_parent_scope_init(int condition) {
    int x;
    x = 42;
    if (condition) {
        process(x);
    }
}
