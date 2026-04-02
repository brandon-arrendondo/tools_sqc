/*
 * Rule: MSC13-C
 * Status: PASS - extern and typedef declarations should not be flagged
 */

extern int global_var;
typedef int my_int;

void f(void) {
    my_int x = 42;
    global_var = x;
}
