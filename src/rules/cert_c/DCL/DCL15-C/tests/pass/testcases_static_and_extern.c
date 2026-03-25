/*
 * Rule: DCL15-C
 * Source: testcases
 * Status: PASS - Proper use of static and extern
 */

/* Static variable — correct internal linkage */
static int internal_counter = 0;

/* Extern declaration — intentionally external */
extern int shared_value;

/* Static function — correct internal linkage */
static void helper(void) {
    internal_counter++;
}

/* main is always external */
int main(void) {
    helper();
    return 0;
}
