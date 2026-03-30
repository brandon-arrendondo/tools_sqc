/*
 * Rule: FIO10-C
 * Source: wiki
 * Status: PASS - access()+remove() before rename(), plus POSIX error-checked rename()
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
