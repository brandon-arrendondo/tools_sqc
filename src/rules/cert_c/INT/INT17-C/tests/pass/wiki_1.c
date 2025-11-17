/*
 * Rule: INT17-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT17-C violation
 */

/* (Correct) Set all bits in mask to 1 */
const unsigned long mask = -1;

unsigned long flipbits(unsigned long x) {
  return x ^ mask;
}