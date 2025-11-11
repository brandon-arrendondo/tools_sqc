/*
 * Rule: FIO10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO10-C violation
 */

const char *src_file = /* ... */;
const char *dest_file = /* ... */;

if (_access_s(dest_file, 0) == 0) {
  if (remove(dest_file) != 0) {
    /* Handle error condition */
  }
}

if (rename(src_file, dest_file) != 0) {
  /* Handle error condition */
}