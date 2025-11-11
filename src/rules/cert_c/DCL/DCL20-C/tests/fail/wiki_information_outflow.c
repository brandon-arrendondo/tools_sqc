/*
 * Rule: DCL20-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL20-C violation
 */

/* Compile using gcc4.3.3 */
void foo() {
  /* 
   * Use assembly code to retrieve i
   * implicitly from caller
   * and transfer it to a less privileged file.
   */
}

...

/* Caller */
foo(i); /* i is fed from user input */