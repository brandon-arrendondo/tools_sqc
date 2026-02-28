/*
 * Rule: INT31-C
 * Source: custom
 * Status: FAIL - Should trigger INT31-C violation
 * Description: int assigned to char without bounds check
 */

void func(int data) {
    char c = data;  /* Violation: int → char */
    (void)c;
}
