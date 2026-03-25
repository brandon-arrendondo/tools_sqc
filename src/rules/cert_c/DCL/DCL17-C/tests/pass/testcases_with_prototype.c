/*
 * Rule: DCL17-C
 * Source: testcases
 * Status: PASS - Function declarations with proper prototypes
 */

/* Proper prototype with void */
int func_void(void);

/* Proper prototype with params */
int add(int a, int b);

/* Definition with prototype */
int multiply(int a, int b) {
    return a * b;
}
