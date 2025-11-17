/*
 * Rule: DCL03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL03-C violation
 */

#include <assert.h>
 
struct timer {
  unsigned char MODE;
  unsigned int DATA;
  unsigned int COUNT;
};

static_assert(sizeof(struct timer) == sizeof(unsigned char) + sizeof(unsigned int) + sizeof(unsigned int),
              "Structure must not have any padding");