/*
 * Rule: INT17-C
 * Source: wiki
 * Status: FAIL - Should trigger INT17-C violation
 */

/* (Incorrect) Set all bits in mask to 1 */
const unsigned long mask = 0xFFFFFFFF;

unsigned long flipbits(unsigned long x) {
  return x ^ mask;
}