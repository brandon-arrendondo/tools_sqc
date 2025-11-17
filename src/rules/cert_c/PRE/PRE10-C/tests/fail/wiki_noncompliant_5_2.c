/*
 * Rule: PRE10-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE10-C violation
 */

if (x > y)
  SWAP(x, y);          /* Branch 1 */
else  
  do_something();     /* Branch 2 */