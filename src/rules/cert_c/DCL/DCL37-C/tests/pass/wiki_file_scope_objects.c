/*
 * Rule: DCL37-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL37-C violation
 */

#include <stddef.h>

static const size_t max_limit = 1024;
size_t limit = 100;

unsigned int getValue(unsigned int count) {
  return count < limit ? count : limit;
}