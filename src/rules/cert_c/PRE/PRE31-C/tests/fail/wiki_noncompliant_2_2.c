/*
 * Rule: PRE31-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE31-C violation
 *
 * This shows the result of expanding ABS(++n) - demonstrating why
 * side effects in unsafe macros are dangerous.
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))

void func(void) {
    int n = 5;
    int m;
    // This macro expands to: m = (((++n) < 0) ? -(++n) : (++n));
    // The ++n is evaluated multiple times - VIOLATION
    m = ABS(++n);
}