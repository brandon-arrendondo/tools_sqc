/*
 * Rule: PRE10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE10-C violation
 */

/*
 * Swaps two values and requires
 * tmp variable to be defined.
 */
#define SWAP(x, y) \
  do { \
    tmp = (x); \
    (x) = (y); \
    (y) = tmp; } \
  while (0)