/*
 * Rule: DCL20-C
 * Source: wiki (information outflow variant)
 * Status: FAIL - Should trigger DCL20-C violation
 *
 * Function declaration with empty parameter list () allows
 * passing arbitrary arguments. Should use (void) to explicitly
 * indicate no parameters.
 */

/* Declaration with empty params — unspecified argument count */
void foo();

void bar(void) {
  foo(42); /* Caller passes argument to foo — allowed by empty () decl */
}
