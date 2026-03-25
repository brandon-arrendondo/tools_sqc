/*
 * Rule: DCL17-C
 * Source: testcases
 * Status: FAIL - K&R style declarations and definitions
 */

/* Empty parameter list — K&R style, not a prototype */
int old_style_func();

/* Function defined without prototype */
void no_proto(x)
    int x;
{
    (void)x;
}
