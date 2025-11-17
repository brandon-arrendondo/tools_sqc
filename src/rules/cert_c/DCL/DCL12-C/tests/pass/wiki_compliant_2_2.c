/*
 * Rule: DCL12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL12-C violation
 */

struct string_mx {
  size_t size;
  size_t maxsize;
  unsigned char strtype;
  char *cstr;
};