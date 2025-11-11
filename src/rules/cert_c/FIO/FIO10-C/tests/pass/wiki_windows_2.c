/*
 * Rule: FIO10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO10-C violation
 */

const char *src_file = /* ... */;
const char *dest_file = /* ... */;

if (!MoveFileEx(src_file, dest_file, MOVEFILE_REPLACE_EXISTING)) {
  /* Handle error condition */
}