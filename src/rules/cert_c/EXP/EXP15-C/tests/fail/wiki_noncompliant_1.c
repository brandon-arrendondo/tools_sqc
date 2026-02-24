/*
 * Rule: EXP15-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP15-C violation
 *
 * Semicolon after if condition creates empty statement;
 * the block executes unconditionally.
 */

void foo(int a, int b) {
  if (a == b); {
    /* This block always executes regardless of condition */
    a = b + 1;
  }
}
