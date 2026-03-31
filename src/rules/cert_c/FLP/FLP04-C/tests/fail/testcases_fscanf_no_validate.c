/*
 * Rule: FLP04-C
 * Source: testcases
 * Status: FAIL - Should trigger FLP04-C violation
 *
 * scanf() reads float without checking for exceptional values
 */

double total;
void process_input() {
    float val;

    /* VIOLATION: no exceptional-value check before use */
    scanf("%f", &val);

    total += val;
}
