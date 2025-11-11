/*
 * Rule: ERR02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR02-C violation
 */

errno_t sprintf_m(
  string_m buf, 
  const string_m fmt, 
  int *count, 
  ...
);