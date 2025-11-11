/*
 * Rule: FIO10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO10-C violation
 */

const char *src_file = /* ... */;
const char *dest_file = /* ... */;

(void)remove(dest_file);

if (rename(src_file, dest_file) != 0) {
  /* Handle error condition */
}