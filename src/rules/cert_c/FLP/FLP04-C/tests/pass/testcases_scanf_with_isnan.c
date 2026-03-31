/*
 * Rule: FLP04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FLP04-C violation
 *
 * scanf() float input validated with isinf() and isnan()
 */

#include <stdio.h>
#include <math.h>

void validated_scanf_input(void) {
    float val;
    scanf("%f", &val);

    /* COMPLIANT: check for exceptional values before use */
    if (isnan(val) || isinf(val)) {
        fprintf(stderr, "Invalid input\n");
        return;
    }
    float result = val * 2.0f;
    printf("Result: %f\n", result);
}
