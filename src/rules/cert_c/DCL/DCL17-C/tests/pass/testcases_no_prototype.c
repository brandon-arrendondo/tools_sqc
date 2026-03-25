/*
 * Rule: DCL17-C
 * Source: testcases
 * Status: PASS - Known limitation: pattern not detected
 * TODO: Move to fail/ when implemented (see PLAN.md)
 */

/* Empty parameter list — K&R style, not a prototype */
int old_style_func();

/* Function defined without prototype */
void no_proto(x)
    int x;
{
    (void)x;
}
