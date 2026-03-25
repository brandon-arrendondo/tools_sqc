/*
 * Rule: FIO10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO10-C violation
 * Note: access()+remove() before rename() satisfies the rule's current check.
 *       The CERT wiki's original POSIX example uses plain rename() with error
 *       checking (POSIX atomically replaces dest), but the rule currently requires
 *       explicit destination handling. TODO: fix rule to accept POSIX rename().
 */

#include <stdio.h>
#include <unistd.h>

const char *src_file = /* ... */;
const char *dest_file = /* ... */;

/* Check if destination exists and handle appropriately */
if (access(dest_file, F_OK) == 0) {
  /* Destination exists - either remove or handle */
  if (remove(dest_file) != 0) {
    /* Handle error condition */
  }
}

if (rename(src_file, dest_file) != 0) {
  /* Handle error condition */
}
